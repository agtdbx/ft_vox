#include <program/map/Map.hpp>

#include <unistd.h>

//**** STATIC FUNCTIONS DEFINE *************************************************

static void	threadRoutine(ThreadData *threadData);

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Map::Map(void)
{
	this->threads = NULL;
	this->threadsData = NULL;
}


Map::Map(const Map &obj)
{
	this->chunks = obj.chunks;
	this->clusters = obj.clusters;
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
	std::size_t	hash = gm::hashSmall(gm::Vec2i(x, y));
	this->chunksMutex.lock();
	ChunkMap::iterator	chunkFind = this->chunks.find(hash);
	this->chunksMutex.unlock();

	if (chunkFind != this->chunks.end())
		return (&chunkFind->second);

	return (NULL);
}


CubeBitmap	*Map::getChunkBitmap(int x, int y)
{
	std::size_t	hash = gm::hashSmall(gm::Vec2i(x, y));
	this->chunksMutex.lock();
	ChunkMap::iterator	chunkFind = this->chunks.find(hash);
	this->chunksMutex.unlock();

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
	printf("Nb threads : %i\n", MAP_NB_THREAD);

	// Init generation info
	this->threadsData = new ThreadData[MAP_NB_THREAD];
	for (int i = 0; i < MAP_NB_THREAD; i++)
	{
		this->threadsData[i].status = THREAD_RUNNING;
		this->threadsData[i].minChunkId = this->minChunkIdOffset;
		this->threadsData[i].maxChunkId = this->maxChunkIdOffset;
		this->threadsData[i].cameraChunkId = cameraChunkId;
		this->threadsData[i].chunks = &this->chunks;
		this->threadsData[i].chunksMutex = &this->chunksMutex;
		this->threadsData[i].clusters = &this->clusters;
		this->threadsData[i].clustersMutex = &this->clustersMutex;
		this->threadsData[i].map = this;
		this->threadsData[i].engine = &engine;
		this->threadsData[i].camera = &camera;
		this->threadsData[i].chunkShader = &chunkShader;
		this->threadsData[i].threadId = i;
	}

	// Create threads
	this->threads = new std::thread[MAP_NB_THREAD];
	for (int i = 0; i < MAP_NB_THREAD; i++)
	{
		this->threads[i] = std::thread(threadRoutine, &this->threadsData[i]);
	}
}


void	Map::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	this->clustersMutex.lock();
	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
	{
		if (camera.isCubeInFrutum(this->clusters[i].getBoundingCube()))
			this->clusters[i].draw(engine, camera, chunkShader);
	}

	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
	{
		if (camera.isCubeInFrutum(this->clusters[i].getBoundingCube()))
			this->clusters[i].drawLiquid(engine, camera, chunkShader);
	}
	this->clustersMutex.unlock();
}


void	Map::destroy(Engine &engine)
{
	this->minDelete = this->cameraChunkId + this->minChunkIdOffset - gm::Vec2i(1, 1);
	this->maxDelete = this->cameraChunkId + this->maxChunkIdOffset + gm::Vec2i(1, 1);
	this->currentView.tmpId = this->minDelete;

	// Destroy chunk multhreaded
	while (1)
	{
		if (this->destroyingChunks())
			break;
		usleep(10000);
	}

	// Free memory into map
	for (int x = this->minDelete.x; x < this->maxDelete.x; x++)
	{
		for (int y = this->minDelete.y; y < this->maxDelete.y; y++)
		{
			std::size_t	hash = gm::hashSmall(gm::Vec2i(x, y));

			if (this->chunks.find(hash) != this->chunks.end())
				this->chunks.erase(hash);
		}
	}

	// Save clean
	ChunkMap::iterator	it = this->chunks.begin();
	while (it != this->chunks.end())
	{
		it->second.destroy(engine);
		it++;
	}

	// Free threads
	if (this->threads)
	{
		for (int i = 0; i < MAP_NB_THREAD; i++)
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

	this->stagingBuffer.destroy(engine.context.getDevice());
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static void	threadRoutine(ThreadData *threadData)
{
	ThreadStatus	status = THREAD_RUNNING;
	gm::Vec2i		minId, maxId, curId;
	StagingBuffer	stagingBuffer;

	int						threadId = threadData->threadId;
	ChunkMap				&chunks = *threadData->chunks;
	std::mutex				&chunksMutex = *threadData->chunksMutex;
	std::vector<Cluster>	&clusters = *threadData->clusters;
	std::mutex				&clustersMutex = *threadData->clustersMutex;
	Map						&map = *threadData->map;
	Engine					&engine = *threadData->engine;
	Camera					&camera = *threadData->camera;
	ChunkShader				&chunkShader = *threadData->chunkShader;
	VulkanCommandPool		&commandPool = engine.commandPoolThreads[threadId];

	stagingBuffer.create(engine.commandPoolThreads[threadId], MAX_CHUNK_BUFFER_SIZE * MIN_CHUNK_PER_THREAD); // Big size to avoid buffer creation

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
					chunksMutex.lock();
					ChunkMap::iterator	it = chunks.find(gm::hashSmall(curId));
					chunksMutex.unlock();

					if (it == chunks.end())
					{
						int i = 0;
						while  (i < 10)
						{
							usleep(1000);
							chunksMutex.lock();
							it = chunks.find(gm::hashSmall(gm::Vec2i(x, y)));
							chunksMutex.unlock();

							if (it == chunks.end())
								i++;
							else
								break;
						}

						if (it == chunks.end())
							continue;
					}

					try
					{
						it->second.init(engine, camera, chunkShader);
						it->second.generate(curId);
					}
					catch(const std::exception& e)
					{
					}
				}
			}

			threadData->mutex.lock();
			if (threadData->status != THREAD_STOPPING)
				threadData->status = THREAD_RUNNING;
			threadData->mutex.unlock();
		}

		else if (status == THREAD_NEED_MESH)
		{
			threadData->mutex.lock();
			threadData->status = THREAD_MESHING;
			minId = threadData->minChunkId;
			maxId = threadData->maxChunkId;
			threadData->mutex.unlock();

			int	nbChunks = (maxId.x - minId.x) * (maxId.y - minId.y);
			VkDeviceSize	minimalSize = nbChunks * MAX_CHUNK_BUFFER_SIZE;

			stagingBuffer.update(commandPool, minimalSize);
			stagingBuffer.offset = 0;
			VkCommandBuffer	commandBuffer = commandPool.beginSingleTimeCommands();

			for (int x = minId.x; x < maxId.x; x++)
			{
				for (int y = minId.y; y < maxId.y; y++)
				{
					chunksMutex.lock();
					ChunkMap::iterator	it = chunks.find(gm::hashSmall(gm::Vec2i(x, y)));
					chunksMutex.unlock();

					if (it == chunks.end())
						continue;

					try
					{
						it->second.createMeshes(map);

						if (stagingBuffer.offset + it->second.getBufferSize() >= stagingBuffer.size)
						{
							engine.queueMutex.lock();
							commandPool.endSingleTimeCommands(commandBuffer);
							engine.queueMutex.unlock();
							commandBuffer = commandPool.beginSingleTimeCommands();
							stagingBuffer.offset = 0;
						}
						it->second.createBuffers(commandPool, stagingBuffer, commandBuffer);
					}
					catch(const std::exception& e)
					{
					}
				}
			}

			engine.queueMutex.lock();
			vkQueueWaitIdle(engine.context.getTransferQueue().value);
			commandPool.endSingleTimeCommands(commandBuffer);
			engine.queueMutex.unlock();

			for (int x = minId.x; x < maxId.x; x++)
			{
				for (int y = minId.y; y < maxId.y; y++)
				{
					gm::Vec2i	chunkPos(x, y);

					chunksMutex.lock();
					ChunkMap::iterator	it = chunks.find(gm::hashSmall(chunkPos));
					chunksMutex.unlock();

					if (it == chunks.end())
						continue;

					it->second.setDrawable(true);

					clustersMutex.lock();
					for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
					{
						if (clusters[i].giveChunk(chunkPos, &it->second))
							break;
					}
					clustersMutex.unlock();
				}
			}

			threadData->mutex.lock();
			if (threadData->status != THREAD_STOPPING)
				threadData->status = THREAD_RUNNING;
			threadData->mutex.unlock();
		}

		else if (status == THREAD_NEED_DESTROY)
		{
			threadData->mutex.lock();
			threadData->status = THREAD_DESTROYING;
			minId = threadData->minChunkId;
			maxId = threadData->maxChunkId;
			threadData->mutex.unlock();

			for (int x = minId.x; x < maxId.x; x++)
			{
				for (int y = minId.y; y < maxId.y; y++)
				{
					chunksMutex.lock();
					ChunkMap::iterator	it = chunks.find(gm::hashSmall(gm::Vec2i(x, y)));
					chunksMutex.unlock();

					if (it == chunks.end())
					{
						continue;
					}

					try
					{
						it->second.destroy(engine);
					}
					catch(const std::exception& e)
					{
					}
				}
			}

			threadData->mutex.lock();
			if (threadData->status != THREAD_STOPPING)
				threadData->status = THREAD_RUNNING;
			threadData->mutex.unlock();
		}

		else
		{
			// Sleep until next loop
			usleep(10000);
		}
	}

	stagingBuffer.destroy(engine.context.getDevice());

	threadData->mutex.lock();
	threadData->status = THREAD_STOP;
	threadData->mutex.unlock();
}
