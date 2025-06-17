#include <program/map/Map.hpp>

#include <unistd.h>

//**** STATIC FUNCTIONS DEFINE *************************************************

static void	threadRoutine(GenerationProcess *generationProcess);
static void	generateChunk(GenerationProcess *generationProcess);

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Map::Map(void)
{
	this->generationThread = NULL;
	this->checkGeneration = false;
}


Map::Map(const Map &obj)
{
	this->chunks = obj.chunks;
	this->clusters = obj.clusters;
	this->generationThread = NULL;
	this->checkGeneration = false;
}

//---- Destructor --------------------------------------------------------------

Map::~Map()
{
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

Chunk	*Map::getChunk(int x, int y)
{
	std::size_t	hash = gm::hash(gm::Vec2i(x, y));

	ChunkMap::iterator	chunkFind = this->chunks.find(hash);

	if (chunkFind != this->chunks.end())
		return (&chunkFind->second);

	return (NULL);
}

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

Map	&Map::operator=(const Map &obj)
{
	if (this == &obj)
		return (*this);

	this->chunks = obj.chunks;
	this->clusters = obj.clusters;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Map::init(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader)
{
	// Create clusters
	this->clusters.resize(MAP_CLUSTER_SIZE);
	this->clusterOffsets.resize(MAP_CLUSTER_SIZE);
	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
		this->clusters[i] = Cluster();

	// Create chunks offsets
	int	nbChunkArround = CLUSTER_SIZE / 2 + MAP_CLUSTER_ARROUND * CLUSTER_SIZE;
	this->minChunkIdOffset = gm::Vec2i(-nbChunkArround, -nbChunkArround);
	this->maxChunkIdOffset = gm::Vec2i( nbChunkArround,  nbChunkArround);

	// Create clusters offsets
	int	minClusterId = -MAP_CLUSTER_ARROUND * CLUSTER_SIZE;
	int	idY, chunkX, chunkY;
	for (int y = 0; y < MAP_CLUSTER_WIDTH; y++)
	{
		chunkY = minClusterId + y * CLUSTER_SIZE;
		idY = y * MAP_CLUSTER_WIDTH;
		for (int x = 0; x < MAP_CLUSTER_WIDTH; x++)
		{
			chunkX = minClusterId + x * CLUSTER_SIZE;
			this->clusterOffsets[x + idY] = gm::Vec2i(chunkX, chunkY);
		}
	}

	// Init generation info
	this->generationProcess.running = true;
	this->generationProcess.mustRun = true;
	this->generationProcess.generating = true;
	this->generationProcess.mustGenerate = true;
	this->generationProcess.minChunkId = this->minChunkIdOffset;
	this->generationProcess.maxChunkId = this->maxChunkIdOffset;
	this->generationProcess.cameraChunkId = gm::Vec2i(0, 0);
	this->generationProcess.chunks = &this->chunks;
	this->generationProcess.map = this;
	this->generationProcess.engine = &engine;
	this->generationProcess.camera = &camera;
	this->generationProcess.chunkShader = &chunkShader;
	this->checkGeneration = true;

	// Create thread
	this->generationThread = new std::thread(threadRoutine, &this->generationProcess);
}


void	Map::update(Camera &camera)
{
	bool	mapChunksToClusters = false;

	if (this->checkGeneration)
	{
		this->generationProcess.mutex.lock();

		if (!this->generationProcess.mustGenerate && !this->generationProcess.generating)
			mapChunksToClusters = true;

		this->generationProcess.mutex.unlock();

		if (mapChunksToClusters)
		{
			this->checkGeneration =  false;
			this->mapChunksIntoClusters();
		}
	}
}


void	Map::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	int	drawCall = 0;

	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
	{
		if (camera.isCubeInFrutum(this->clusters[i].getBoundingCube()))
			this->clusters[i].draw(engine, camera, chunkShader, drawCall);
	}

	// TODO: Remove draw call count
	if (engine.inputManager.l.isPressed())
	{
		int	nbChunkArround = CLUSTER_SIZE / 2 + MAP_CLUSTER_ARROUND * CLUSTER_SIZE;
		int	totalChunks = (nbChunkArround * 2) * (nbChunkArround * 2);

		printf("Draw call : %i / %i\n", drawCall, totalChunks);
	}
}


void	Map::destroy(Engine &engine)
{
	if (this->generationThread)
	{
		// Ask thread to  stop
		this->generationProcess.mutex.lock();
		this->generationProcess.mustRun = false;
		this->generationProcess.mutex.unlock();

		// Wait the thread stop
		while (42)
		{
			usleep(100000);
			this->generationProcess.mutex.lock();
			if (!this->generationProcess.running)
			{
				this->generationProcess.mutex.unlock();
				break;
			}
			this->generationProcess.mutex.unlock();
		}

		// Delete thread
		this->generationThread->join();
		delete this->generationThread;
		this->generationThread = NULL;
	}

	for (int i = 0; i < CLUSTER_SIZE2; i++)
		this->chunks[i].destroy(engine);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Map::mapChunksIntoClusters(void)
{
	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
	{
		this->clusters[i].setChunks(*this, this->clusterOffsets[i]);
	}
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static void	threadRoutine(GenerationProcess *generationProcess)
{
	bool	running = true;
	bool	generate = false;

	while (running)
	{
		// Check thread instruction
		generationProcess->mutex.lock();
		running = generationProcess->mustRun;
		generate = generationProcess->mustGenerate;
		generationProcess->mutex.unlock();

		if (generate)
		{
			generateChunk(generationProcess);

			generationProcess->mutex.lock();
			generationProcess->generating = false;
			generationProcess->mustGenerate = false;
			generationProcess->mutex.unlock();
		}

		// Sleep until next loop
		usleep(100000);
	}

	generationProcess->mutex.lock();
	generationProcess->running = false;
	generationProcess->mutex.unlock();
}


static void	generateChunk(GenerationProcess *generationProcess)
{
	static ChunkMap	&chunks = *generationProcess->chunks;
	static Map			&map = *generationProcess->map;
	static Engine		&engine = *generationProcess->engine;
	static Camera		&camera = *generationProcess->camera;
	static ChunkShader	&chunkShader = *generationProcess->chunkShader;

	PerfLogger	perfLogger;
	resetLog(perfLogger.generation);
	resetLog(perfLogger.createMesh);
	perfLogger.nbTriangles = 0;

	gm::Vec2i	minChunk = generationProcess->minChunkId + generationProcess->cameraChunkId;
	gm::Vec2i	maxChunk = generationProcess->maxChunkId + generationProcess->cameraChunkId;
	std::size_t	hash;

	// Generate chunks
	int	nbChunks = 0;
	int	nbChunkArround = CLUSTER_SIZE / 2 + MAP_CLUSTER_ARROUND * CLUSTER_SIZE;
	int	totalChunks = (nbChunkArround * 2) * (nbChunkArround * 2);
	printf("Chunks generation :\n");
	for (int x = minChunk.x; x < maxChunk.x; x++)
	{
		for (int y = minChunk.y; y < maxChunk.y; y++)
		{
			nbChunks++;
			printf("\r chunk %i/%i", nbChunks, totalChunks);

			hash = gm::hash(gm::Vec2i(x, y));
			if (chunks.find(hash) != chunks.end())
				continue;

			chunks[hash] = Chunk();
			chunks[hash].init(engine, camera, chunkShader);
			chunks[hash].generate(gm::Vec2i(x, y), perfLogger);
		}
	}

	printf("\nChunks mesh creation :\n");
	nbChunks = 0;
	// Create chunks meshes
	for (int x = minChunk.x; x < maxChunk.x; x++)
	{
		for (int y = minChunk.y; y < maxChunk.y; y++)
		{
			nbChunks++;
			printf("\r chunk %i/%i", nbChunks, totalChunks);

			hash = gm::hash(gm::Vec2i(x, y));
			chunks[hash].updateMeshes(map, perfLogger);
		}
	}
	printf("\n\n");

	printLog(perfLogger.generation, "Chunk generation");
	printLog(perfLogger.createMesh, "Mesh creation");
	printf("Number of triangle : %i\n", perfLogger.nbTriangles);
}
