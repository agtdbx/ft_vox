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

	this->cameraChunkId = gm::Vec2i(0, 0); // TODO : Reput camera pos
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


void	Map::update(Engine &engine, Camera &camera)
{
	static MapStatus	status = MAP_NONE;
	ThreadStatus		threadStatus;
	static int			nbGeneration = 0;
	static int			nbMesh = 0;

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
		int		totalHeightGenerate = this->targetView.maxGenChunk.y - this->targetView.minGenChunk.y;
		int		totalGenerate = totalWidthGenerate * totalHeightGenerate;
		int		widthGeneratePerThread = gm::max(4, totalWidthGenerate / MAP_NB_THREAD);

		for (int i = 0; i < MAP_NB_THREAD; i++)
		{
			this->threadsData[i].mutex.lock();
			threadStatus = this->threadsData[i].status;
			this->threadsData[i].mutex.unlock();

			// Update when thread end generating asked chunks
			if (threadStatus == THREAD_GENERATE_END)
			{
				nbGeneration += this->threadsData[i].maxChunkId.x - this->threadsData[i].minChunkId.x;
				std::cout << "\r  - generate " << nbGeneration << " / " << totalGenerate << std::flush;

				threadStatus = THREAD_RUNNING;
			}

			if (threadStatus == THREAD_RUNNING)
			{
				if (this->currentView.tmpId.y != this->targetView.maxGenChunk.y)
				{
					int	chunkLeftBeforeEndLine = this->targetView.maxGenChunk.x - this->currentView.tmpId.x;

					gm::Vec2i	minId = this->currentView.tmpId;
					this->currentView.tmpId += gm::Vec2i(gm::min(chunkLeftBeforeEndLine, widthGeneratePerThread), 0);
					gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(0, 1);

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
			std::cout << "\n\nBegin meshing :" << std::endl;

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
		int		totalWidthMesh = this->targetView.maxMeshChunk.x - this->targetView.minMeshChunk.x;
		int		totalHeightMesh = this->targetView.maxMeshChunk.y - this->targetView.minMeshChunk.y;
		int		totalMesh = totalWidthMesh * totalHeightMesh;
		int		widthMeshPerThread = gm::min(gm::max(4, totalWidthMesh / MAP_NB_THREAD), 8);

		for (int i = 0; i < MAP_NB_THREAD; i++)
		{
			this->threadsData[i].mutex.lock();
			threadStatus = this->threadsData[i].status;
			this->threadsData[i].mutex.unlock();

			// Update when thread end creating mesh for asked chunks
			if (threadStatus == THREAD_MESH_END)
			{
				gm::Vec2i	minId = this->threadsData[i].minChunkId;
				gm::Vec2i	maxId = this->threadsData[i].maxChunkId;

				nbMesh += maxId.x - minId.x;
				std::cout << "\r  - mesh " << nbMesh << " / " << totalMesh << std::flush;

				for (int x = minId.x; x < maxId.x; x++)
				{
					for (int y = minId.y; y < maxId.y; y++)
					{
						gm::Vec2i	chunkPos = gm::Vec2i(x, y);
						ChunkMap::iterator	it = this->chunks.find(gm::hash(chunkPos));

						if (it == this->chunks.end())
							continue;

						for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
						{
							this->clusters[i].giveChunk(chunkPos, &it->second);
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
					gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(0, 1);

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
			std::cout << "\nEnd generate chunks" << std::endl;

			this->currentView.maxMeshChunk = this->targetView.maxMeshChunk;
			this->currentView.tmpId = this->targetView.tmpId;

			status = MAP_NONE;

			printf("Nb generation %i\n", nbGeneration);
			printf("Nb mesh %i\n", nbMesh);
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
	PerfLogger	perfLogger;

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
					perfLogger += this->threadsData[i].perfLogger;
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

	this->stagingBuffer.destroy(engine.context.getDevice());

	printf("\nTerrain generation stats !\n\n");
	printf("Generation :\n");
	perflogPrint(perfLogger.generateChunk,       " - per chunk     ");
	printf("Meshing :\n");
	perflogPrint(perfLogger.chunkMeshing,        " - per chunk     ");
	perflogPrint(perfLogger.meshChunk,           " - border mesh   ");
	perflogPrint(perfLogger.meshBlock,           " - block mesh    ");
	perflogPrint(perfLogger.meshBlockCopyBitmap, " - copy bitmap   ");
	perflogPrint(perfLogger.meshBlockXaxis,      " - block x axis  ");
	perflogPrint(perfLogger.meshBlockYaxis,      " - block y axis  ");
	perflogPrint(perfLogger.meshBlockZaxis,      " - block z axis  ");
	perflogPrint(perfLogger.meshWater,           " - water mesh    ");
	printf("Buffering :\n");
	perflogPrint(perfLogger.createBuffer,        " - per chunk     ");
	perflogPrint(perfLogger.createUpdateStaging, " - staging buffer");
	perflogPrint(perfLogger.mapVertexBuffer,     " - vertex map    ");
	perflogPrint(perfLogger.createVertexBuffer,  " - vertex create ");
	perflogPrint(perfLogger.copyVertexBuffer,    " - vertex copy   ");
	perflogPrint(perfLogger.mapIndexBuffer,      " - index map     ");
	perflogPrint(perfLogger.createIndexBuffer,   " - index create  ");
	perflogPrint(perfLogger.copyIndexBuffer,     " - index copy    ");

	// TODO : REMOVE Print into csv format
	// printf("\nCsv format !\n");
	// printf("Name;Total time (us);Nb call;Avg time (us);\n");
	// perflogPrintCsv(perfLogger.generateChunk,       "Generation per chunk");
	// perflogPrintCsv(perfLogger.chunkMeshing,        "Meshing per chunk");
	// perflogPrintCsv(perfLogger.meshChunk,           "Meshing border mesh");
	// perflogPrintCsv(perfLogger.meshBlock,           "Meshing block mesh");
	// perflogPrintCsv(perfLogger.meshBlockCopyBitmap, "Meshing copy bitmap");
	// perflogPrintCsv(perfLogger.meshBlockXaxis,      "Meshing block x axis");
	// perflogPrintCsv(perfLogger.meshBlockYaxis,      "Meshing block y axis");
	// perflogPrintCsv(perfLogger.meshBlockZaxis,      "Meshing block z axis");
	// perflogPrintCsv(perfLogger.meshWater,           "Meshing water mesh");
	// perflogPrintCsv(perfLogger.createBuffer,        "Buffering per chunk");
	// perflogPrintCsv(perfLogger.createUpdateStaging, "Buffering staging buffer");
	// perflogPrintCsv(perfLogger.mapVertexBuffer,     "Buffering vertex map");
	// perflogPrintCsv(perfLogger.createVertexBuffer,  "Buffering vertex create");
	// perflogPrintCsv(perfLogger.copyVertexBuffer,    "Buffering vertex copy");
	// perflogPrintCsv(perfLogger.mapIndexBuffer,      "Buffering index map");
	// perflogPrintCsv(perfLogger.createIndexBuffer,   "Buffering index create");
	// perflogPrintCsv(perfLogger.copyIndexBuffer,     "Buffering index copy");
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

#define MAX_CHUNK_BUFFER_SIZE 150000

static void	threadRoutine(ThreadData *threadData)
{
	ThreadStatus	status = THREAD_RUNNING;
	std::size_t		hash;
	gm::Vec2i		minId, maxId, curId;
	StagingBuffer	stagingBuffer;

	int					threadId = threadData->threadId;
	ChunkMap			&chunks = *threadData->chunks;
	Map					&map = *threadData->map;
	Engine				&engine = *threadData->engine;
	Camera				&camera = *threadData->camera;
	ChunkShader			&chunkShader = *threadData->chunkShader;
	VulkanCommandPool	&commandPool = engine.commandPoolThreads[threadId];
	PerfLogger			&perfLogger = threadData->perfLogger;

	stagingBuffer.create(engine.commandPoolThreads[threadId], MAX_CHUNK_BUFFER_SIZE * 8); // Big size to avoid buffer creation

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
					ChunkMap::iterator	it = chunks.find(hash);

					if (it == chunks.end())
						continue;

					it->second.init(engine, camera, chunkShader);
					it->second.generate(curId, perfLogger);
				}
			}

			threadData->mutex.lock();
			if (threadData->status != THREAD_STOPPING)
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

			int	nbChunks = (maxId.x - minId.x) * (maxId.y - minId.y);
			VkDeviceSize	minimalSize = nbChunks * MAX_CHUNK_BUFFER_SIZE;

			stagingBuffer.update(commandPool, minimalSize, perfLogger);
			stagingBuffer.offset = 0;
			VkCommandBuffer	commandBuffer = commandPool.beginSingleTimeCommands();

			for (int x = minId.x; x < maxId.x; x++)
			{
				for (int y = minId.y; y < maxId.y; y++)
				{
					hash = gm::hash(gm::Vec2i(x, y));
					ChunkMap::iterator	it = chunks.find(hash);

					if (it == chunks.end())
						continue;

					it->second.createMeshes(map, perfLogger);
					it->second.createBuffers(commandPool, stagingBuffer, commandBuffer, perfLogger);
				}
			}

			commandPool.endSingleTimeCommands(commandBuffer);

			threadData->mutex.lock();
			if (threadData->status != THREAD_STOPPING)
				threadData->status = THREAD_MESH_END;
			threadData->mutex.unlock();
		}

		else
		{
			// Sleep until next loop
			usleep(100000);
		}
	}

	perfLogger.meshBlockXaxis.nbCall = perfLogger.meshBlockYaxis.nbCall;
	perfLogger.meshBlockZaxis.nbCall = perfLogger.meshBlockYaxis.nbCall;
	perfLogger.mapVertexBuffer.nbCall = perfLogger.createBuffer.nbCall;
	perfLogger.createVertexBuffer.nbCall = perfLogger.createBuffer.nbCall;
	perfLogger.copyVertexBuffer.nbCall = perfLogger.createBuffer.nbCall;
	perfLogger.mapIndexBuffer.nbCall = perfLogger.createBuffer.nbCall;
	perfLogger.createIndexBuffer.nbCall = perfLogger.createBuffer.nbCall;
	perfLogger.copyIndexBuffer.nbCall = perfLogger.createBuffer.nbCall;

	stagingBuffer.destroy(engine.context.getDevice());

	threadData->mutex.lock();
	threadData->status = THREAD_STOP;
	threadData->mutex.unlock();
}
