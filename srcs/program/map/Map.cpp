#include <program/map/Map.hpp>

#include <unistd.h>

//**** STATIC FUNCTIONS DEFINE *************************************************

static void	threadRoutine(GenerationProcess *generationProcess);

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


CubeBitmap	*Map::getChunkBitmap(int x, int y)
{
	std::size_t	hash = gm::hash(gm::Vec2i(x, y));

	ChunkMap::iterator	chunkFind = this->chunks.find(hash);

	if (chunkFind != this->chunks.end())
		return (&chunkFind->second.getCubeBitmap());

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

	this->cameraChunkId = gm::Vec2i(camera.getPosition().x / CHUNK_SIZE,
									camera.getPosition().z / CHUNK_SIZE);


	// Init current view
	this->currentView.minGenChunk = this->cameraChunkId;
	this->currentView.maxGenChunk = this->cameraChunkId;
	this->currentView.minMeshChunk = this->cameraChunkId;
	this->currentView.maxMeshChunk = this->cameraChunkId;
	this->currentView.tmpId = this->cameraChunkId;

	// Init target view
	this->targetView.minGenChunk = this->cameraChunkId + this->minChunkIdOffset - gm::Vec2i(1, 1);
	this->targetView.maxGenChunk = this->cameraChunkId + this->maxChunkIdOffset + gm::Vec2i(1, 1);
	this->targetView.minMeshChunk = this->cameraChunkId + this->minChunkIdOffset;
	this->targetView.maxMeshChunk = this->cameraChunkId + this->maxChunkIdOffset;
	this->targetView.tmpId = this->targetView.minGenChunk;

	// Init generation info
	this->generationProcess.status = THREAD_RUNNING;
	this->generationProcess.minChunkId = this->minChunkIdOffset;
	this->generationProcess.maxChunkId = this->maxChunkIdOffset;
	this->generationProcess.cameraChunkId = cameraChunkId;
	this->generationProcess.chunks = &this->chunks;
	this->generationProcess.map = this;
	this->generationProcess.engine = &engine;
	this->generationProcess.camera = &camera;
	this->generationProcess.chunkShader = &chunkShader;
	this->checkGeneration = true;

	// Create thread
	this->generationThread = new std::thread(threadRoutine, &this->generationProcess);
}


void	Map::update(Engine &engine, Camera &camera)
{
	static MapStatus	status = MAP_NONE;

	// TODO : Update targetView if cameraChunkId change

	if (this->currentView == this->targetView)
		return ;

	if (status == MAP_NONE)
	{
		// TODO : Delete useless chunk, update currentView
		std::cout << "\nBegin generation :" << std::endl;

		this->currentView.minGenChunk = this->targetView.minGenChunk;
		this->currentView.maxGenChunk = this->targetView.minGenChunk;
		this->currentView.tmpId = this->targetView.minGenChunk;

		status = MAP_GENERATING;
	}

	if (status == MAP_GENERATING)
	{
		this->generationProcess.mutex.lock(); // TODO : Move code ouside of lock
		// Update when thread end generating asked chunks
		if (this->generationProcess.status == THREAD_GENERATE_END)
		{
			this->generationProcess.status = THREAD_RUNNING;
		}

		if (this->generationProcess.status == THREAD_RUNNING)
		{
			if (this->currentView.tmpId != this->targetView.maxGenChunk)
			{
				if (this->currentView.tmpId.x == this->targetView.maxGenChunk.x)
				{
					this->currentView.tmpId.x = this->targetView.minGenChunk.x;
					this->currentView.tmpId.y++;
				}

				std::cout << "  - generate " << this->currentView.tmpId << std::endl;
				this->generationProcess.minChunkId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(1, 0);
				this->generationProcess.maxChunkId = this->currentView.tmpId;
				this->generationProcess.status = THREAD_NEED_GENERATE;
			}
			else
			{
				// TODO : Check that all thread have finish generating
				std::cout << "\nBegin meshing :" << std::endl;

				this->currentView.maxGenChunk = this->targetView.maxGenChunk;
				this->currentView.minMeshChunk = this->targetView.minMeshChunk;
				this->currentView.maxMeshChunk = this->targetView.minMeshChunk;
				this->currentView.tmpId = this->targetView.minMeshChunk;

				status = MAP_MESHING;
			}
		}
		this->generationProcess.mutex.unlock();
	}

	if (status == MAP_MESHING)
	{
		// TODO : Create mesh
		// TODO : When mesh is created, create buffer
		// TODO : When buffer is create, give chunk to cluster
		// TODO : When all mesh are finish, put map status to none
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

	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
	{
		if (camera.isCubeInFrutum(this->clusters[i].getBoundingCube()))
			this->clusters[i].drawWater(engine, camera, chunkShader);
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
		this->generationProcess.status = THREAD_STOPPING;
		this->generationProcess.mutex.unlock();

		// Wait the thread stop
		while (42)
		{
			usleep(10000);
			this->generationProcess.mutex.lock();
			if (this->generationProcess.status == THREAD_STOP)
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

	ChunkMap::iterator	it = this->chunks.begin();
	while (it != this->chunks.end())
	{
		it->second.destroy(engine);
		it++;
	}
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Map::mapChunksIntoClusters(Engine &engine)
{
	PerfLogger	perfLogger;

	perflogReset(perfLogger.createBuffer);
	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
	{
		this->clusters[i].setChunks(engine, *this, this->clusterOffsets[i] + this->cameraChunkId, perfLogger);
	}
	perflogPrint(perfLogger.createBuffer, "Buffer creation");
	printf("\n");
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static void	threadRoutine(GenerationProcess *generationProcess)
{
	ThreadStatus	status = THREAD_RUNNING;
	Chunk			*chunk;
	std::size_t		hash;
	gm::Vec2i		minId, maxId, curId;

	Map			&map = *generationProcess->map;
	ChunkMap	&chunks = *generationProcess->chunks;
	ChunkShader	&chunkShader = *generationProcess->chunkShader;
	Camera		&camera = *generationProcess->camera;
	Engine		&engine = *generationProcess->engine;


	PerfLogger		perfLogger; // TODO : Remove

	while (status != THREAD_STOPPING)
	{
		// Check thread instruction
		generationProcess->mutex.lock();
		status = generationProcess->status;
		generationProcess->mutex.unlock();

		if (status == THREAD_NEED_GENERATE)
		{
			generationProcess->mutex.lock();
			generationProcess->status = THREAD_GENERATING;
			minId = generationProcess->minChunkId;
			maxId = generationProcess->maxChunkId;
			generationProcess->mutex.unlock();

			for (int x = minId.x; x < maxId.x; x++)
			{
				for (int y = minId.y; y < maxId.y; y++)
				{
					curId = gm::Vec2i(x, y);
					hash = gm::hash(curId);

					// TODO : Put a mutex for put check and put in chunks map
					if (chunks.find(hash) != chunks.end())
						continue;

					chunks[hash] = Chunk();
					chunk = &chunks[hash];

					chunk->init(engine, camera, chunkShader);
					chunk->generate(curId, perfLogger);
				}
			}

			generationProcess->mutex.lock();
			generationProcess->status = THREAD_GENERATE_END;
			generationProcess->mutex.unlock();
		}
		else if (status == THREAD_NEED_MESH)
		{
			generationProcess->mutex.lock();
			generationProcess->status = THREAD_MESHING;
			minId = generationProcess->minChunkId;
			maxId = generationProcess->maxChunkId;
			generationProcess->mutex.unlock();

			for (int x = minId.x; x < maxId.x; x++)
			{
				for (int y = minId.y; y < maxId.y; y++)
				{
					curId = gm::Vec2i(x, y);
					hash = gm::hash(curId);

					// TODO : Put a mutex for put check and put in chunks map
					if (chunks.find(hash) == chunks.end())
						continue;

					chunk = &chunks[hash];

					chunk->createMeshes(map, perfLogger);
				}
			}

			generationProcess->mutex.lock();
			generationProcess->status = THREAD_MESH_END;
			generationProcess->mutex.unlock();
		}
		else
		{
			// Sleep until next loop
			usleep(100000);
		}
	}

	generationProcess->mutex.lock();
	generationProcess->status = THREAD_STOP;
	generationProcess->mutex.unlock();
}
