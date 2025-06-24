#include <program/map/Map.hpp>

#include <unistd.h>

//**** STATIC FUNCTIONS DEFINE *************************************************

static void	threadRoutine(ThreadData *threadData);

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Map::Map(void)
{
	this->nbThread = 0;
	this->threads = NULL;
	this->threadsData = NULL;
}


Map::Map(const Map &obj)
{
	this->chunks = obj.chunks;
	this->clusters = obj.clusters;
	this->nbThread = 0;
	this->threads = NULL;
	this->threadsData = NULL;
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

	this->cameraChunkId = gm::Vec2i(0, 0);
	// this->cameraChunkId = gm::Vec2i(camera.getPosition().x / CHUNK_SIZE,
	// 								camera.getPosition().z / CHUNK_SIZE);


	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
		this->clusters[i].setPosition(this->cameraChunkId + this->clusterOffsets[i]);


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

	// Get nb threads
	// this->nbThread = 1;
	this->nbThread = std::thread::hardware_concurrency() - 1;
	printf("Nb threads : %i\n", this->nbThread);

	// Init generation info
	this->threadsData = new ThreadData[this->nbThread];
	for (int i = 0; i < this->nbThread; i++)
	{
		this->threadsData[i].status = THREAD_RUNNING;
		this->threadsData[i].minChunkId = this->minChunkIdOffset;
		this->threadsData[i].maxChunkId = this->maxChunkIdOffset;
		this->threadsData[i].cameraChunkId = cameraChunkId;
		this->threadsData[i].chunks = &this->chunks;
		this->threadsData[i].map = this;
		this->threadsData[i].engine = &engine;
		this->threadsData[i].camera = &camera;
		this->threadsData[i].chunkShader = &chunkShader;
	}

	// Create threads
	this->threads = new std::thread[this->nbThread];
	for (int i = 0; i < this->nbThread; i++)
	{
		this->threads[i] = std::thread(threadRoutine, &this->threadsData[i]);
	}
}


void	Map::update(Engine &engine, Camera &camera)
{
	static MapStatus	status = MAP_NONE;
	ThreadStatus		threadStatus;
	static PerfLogger	perfLogger; // TODO : remove
	static int			nbGeneration = 0;
	static int			nbMesh = 0;
	static int			nbBuffer = 0;

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
		bool	allGenerationDone = true;
		int		totalWidthGenerate = this->targetView.maxGenChunk.x - this->targetView.minGenChunk.x;
		int		widthGeneratePerThread = gm::max(4, totalWidthGenerate / nbThread);

		for (int i = 0; i < this->nbThread; i++)
		{
			this->threadsData[i].mutex.lock();
			threadStatus = this->threadsData[i].status;
			this->threadsData[i].mutex.unlock();

			// Update when thread end generating asked chunks
			if (threadStatus == THREAD_GENERATE_END)
			{
				threadStatus = THREAD_RUNNING;
			}

			if (threadStatus == THREAD_RUNNING)
			{
				if (this->currentView.tmpId.y != this->targetView.maxGenChunk.y)
				{
					int	chunkLeftBeforeEndLine = this->targetView.maxGenChunk.x - this->currentView.tmpId.x;

					gm::Vec2i	minId = this->currentView.tmpId;
					this->currentView.tmpId += gm::Vec2i(gm::min(chunkLeftBeforeEndLine, widthGeneratePerThread), 0);
					nbGeneration += gm::min(chunkLeftBeforeEndLine, widthGeneratePerThread);
					gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(0, 1);

					std::cout << "  - generate " << minId << " -> " << maxId << std::endl;

					for (int x = minId.x; x < maxId.x; x++)
					{
						for (int y = minId.y; y < maxId.y; y++)
						{
							this->chunks[gm::hash(gm::Vec2i(x, y))] = Chunk();
						}
					}

					this->threadsData[i].mutex.lock();
					this->threadsData[i].minChunkId = minId;
					this->threadsData[i].maxChunkId = maxId;
					this->threadsData[i].status = THREAD_NEED_GENERATE;
					this->threadsData[i].mutex.unlock();

					if (chunkLeftBeforeEndLine == 0)
					{
						this->currentView.tmpId.x = this->targetView.minGenChunk.x;
						this->currentView.tmpId.y++;
					}

					allGenerationDone = false;
				}
				else
				{
					this->threadsData[i].mutex.lock();
					this->threadsData[i].status = THREAD_RUNNING;
					this->threadsData[i].mutex.unlock();
				}
			}
			else
				allGenerationDone = false;
		}

		if (allGenerationDone)
		{
			std::cout << "\nBegin meshing :" << std::endl;

			this->currentView.maxGenChunk = this->targetView.maxGenChunk;
			this->currentView.minMeshChunk = this->targetView.minMeshChunk;
			this->currentView.maxMeshChunk = this->targetView.minMeshChunk;
			this->currentView.tmpId = this->targetView.minMeshChunk;

			status = MAP_MESHING;
		}
	}

	if (status == MAP_MESHING)
	{
		bool	allMeshDone = true;
		// int		totalWidthMesh = this->targetView.maxGenChunk.x - this->targetView.minGenChunk.x;
		// int		widthMeshPerThread = gm::min(gm::max(4, totalWidthMesh / nbThread), 8);
		int		widthMeshPerThread = 1;

		for (int i = 0; i < this->nbThread; i++)
		{
			this->threadsData[i].mutex.lock();
			threadStatus = this->threadsData[i].status;
			this->threadsData[i].mutex.unlock();

			// Update when thread end creating mesh for asked chunks
			if (threadStatus == THREAD_MESH_END)
			{
				this->threadsData[i].mutex.lock();
				gm::Vec2i	minId = this->threadsData[i].minChunkId;
				gm::Vec2i	maxId = this->threadsData[i].maxChunkId;
				this->threadsData[i].mutex.unlock();

				// When mesh is created, create buffer
				for (int x = minId.x; x < maxId.x; x++)
				{
					for (int y = minId.y; y < maxId.y; y++)
					{
						gm::Vec2i	chunkPos = gm::Vec2i(x, y);
						Chunk		*chunk = &this->chunks[gm::hash(chunkPos)];
						chunk->createBuffers(engine.commandPool, perfLogger);
						nbBuffer++;

						for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
						{
							this->clusters[i].giveChunk(chunkPos, chunk);
						}
					}
				}
				threadStatus = THREAD_RUNNING;
			}

			if (threadStatus == THREAD_RUNNING)
			{
				if (this->currentView.tmpId.y != this->targetView.maxMeshChunk.y)
				{
					int	chunkLeftBeforeEndLine = this->targetView.maxMeshChunk.x - this->currentView.tmpId.x;

					gm::Vec2i	minId = this->currentView.tmpId;
					this->currentView.tmpId += gm::Vec2i(gm::min(chunkLeftBeforeEndLine, widthMeshPerThread), 0);
					nbMesh += gm::min(chunkLeftBeforeEndLine, widthMeshPerThread);
					gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(0, 1);

					std::cout << "  - mesh " << minId << " -> " << maxId << std::endl;

					this->threadsData[i].mutex.lock();
					this->threadsData[i].minChunkId = minId;
					this->threadsData[i].maxChunkId = maxId;
					this->threadsData[i].status = THREAD_NEED_MESH;
					this->threadsData[i].mutex.unlock();

					if (chunkLeftBeforeEndLine == 0)
					{
						this->currentView.tmpId.x = this->targetView.minMeshChunk.x;
						this->currentView.tmpId.y++;
					}

					allMeshDone = false;
				}
				else
				{
					this->threadsData[i].mutex.lock();
					this->threadsData[i].status = THREAD_RUNNING;
					this->threadsData[i].mutex.unlock();
				}
			}
			else
				allMeshDone = false;
		}

		if (allMeshDone)
		{
			std::cout << "End generate chunks" << std::endl;

			this->currentView.maxMeshChunk = this->targetView.maxMeshChunk;
			this->currentView.tmpId = this->targetView.tmpId;

			status = MAP_NONE;

			perflogPrint(perfLogger.createBuffer, "Create buffer time");
			printf("Nb generation %i\n", nbGeneration);
			printf("Nb mesh %i\n", nbMesh);
			printf("Nb buffer %i\n", nbBuffer);
		}
	}
}


void	Map::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	int	drawCall = 0;

	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
	{
		this->clusters[i].draw(engine, camera, chunkShader, drawCall);
		// if (camera.isCubeInFrutum(this->clusters[i].getBoundingCube()))
		// 	this->clusters[i].draw(engine, camera, chunkShader, drawCall);
	}

	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
	{
		this->clusters[i].drawWater(engine, camera, chunkShader);
		// if (camera.isCubeInFrutum(this->clusters[i].getBoundingCube()))
		// 	this->clusters[i].drawWater(engine, camera, chunkShader);
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
	if (this->threads)
	{
		for (int i = 0; i < this->nbThread; i++)
		{

			// Ask thread to  stop
			this->threadsData[i].mutex.lock();
			this->threadsData[i].status = THREAD_STOPPING;
			this->threadsData[i].mutex.unlock();

			// Wait the thread stop
			while (42)
			{
				usleep(10000);
				this->threadsData[i].mutex.lock();
				if (this->threadsData[i].status == THREAD_STOP)
				{
					this->threadsData[i].mutex.unlock();
					break;
				}
				this->threadsData[i].mutex.unlock();
			}
			this->threads[i].join();
		}


		// Delete threads
		delete [] this->threadsData;
		delete [] this->threads;
		this->threadsData = NULL;
		this->threads = NULL;
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
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static void	threadRoutine(ThreadData *threadData)
{
	ThreadStatus	status = THREAD_RUNNING;
	std::size_t		hash;
	gm::Vec2i		minId, maxId, curId;

	ChunkMap	&chunks = *threadData->chunks;
	Map			&map = *threadData->map;
	Engine		&engine = *threadData->engine;
	Camera		&camera = *threadData->camera;
	ChunkShader	&chunkShader = *threadData->chunkShader;

	PerfLogger	perfLogger; // TODO : Remove;

	while (status != THREAD_STOPPING)
	{
		// Check thread instruction
		threadData->mutex.lock();
		status = threadData->status;
		threadData->mutex.unlock();

		if (status == THREAD_NEED_GENERATE)
		{
			threadData->mutex.lock();
			threadData->status = THREAD_GENERATING;
			minId = threadData->minChunkId;
			maxId = threadData->maxChunkId;
			threadData->mutex.unlock();

			for (int x = minId.x; x < maxId.x; x++)
			{
				for (int y = minId.y; y < maxId.y; y++)
				{
					curId = gm::Vec2i(x, y);
					hash = gm::hash(curId);

					chunks[hash].init(engine, camera, chunkShader);
					chunks[hash].generate(curId, perfLogger);
				}
			}

			threadData->mutex.lock();
			threadData->status = THREAD_GENERATE_END;
			threadData->mutex.unlock();
		}

		else if (status == THREAD_NEED_MESH)
		{
			threadData->mutex.lock();
			threadData->status = THREAD_MESHING;
			minId = threadData->minChunkId;
			maxId = threadData->maxChunkId;
			threadData->mutex.unlock();

			for (int x = minId.x; x < maxId.x; x++)
			{
				for (int y = minId.y; y < maxId.y; y++)
				{
					curId = gm::Vec2i(x, y);
					hash = gm::hash(curId);

					// TODO : Put a mutex for put check and put in chunks map
					if (chunks.find(hash) == chunks.end())
						continue;

					chunks[hash].createMeshes(map, perfLogger);
				}
			}

			threadData->mutex.lock();
			threadData->status = THREAD_MESH_END;
			threadData->mutex.unlock();
		}

		else
		{
			// Sleep until next loop
			usleep(100000);
		}
	}

	threadData->mutex.lock();
	threadData->status = THREAD_STOP;
	threadData->mutex.unlock();
}
