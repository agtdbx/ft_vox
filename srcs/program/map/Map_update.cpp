#include <program/map/Map.hpp>

#include <unistd.h>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** PUBLIC METHODS **********************************************************

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

		status = MAP_GENERATING_X;
	}

	if (status == MAP_GENERATING_X)
	{
		bool	allGenerationDone = true;
		int		totalWidthGenerate = this->targetView.maxGenChunk.x - this->targetView.minGenChunk.x;
		int		totalHeightGenerate = this->targetView.maxGenChunk.y - this->targetView.minGenChunk.y;
		int		totalGenerate = totalWidthGenerate * totalHeightGenerate;
		int		widthGeneratePerThread = gm::max(MIN_CHUNK_PER_THREAD, totalWidthGenerate / MAP_NB_THREAD);

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

					chunkLeftBeforeEndLine = this->targetView.maxGenChunk.x - this->currentView.tmpId.x;
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

			status = MAP_MESHING_X;
		}
	}

	else if (status == MAP_GENERATING_Y)
	{
		bool	allGenerationDone = true;
		int		totalWidthGenerate = this->targetView.maxGenChunk.x - this->targetView.minGenChunk.x;
		int		totalHeightGenerate = this->targetView.maxGenChunk.y - this->targetView.minGenChunk.y;
		int		totalGenerate = totalWidthGenerate * totalHeightGenerate;
		int		heightGeneratePerThread = gm::max(MIN_CHUNK_PER_THREAD, totalHeightGenerate / MAP_NB_THREAD);

		for (int i = 0; i < MAP_NB_THREAD; i++)
		{
			this->threadsData[i].mutex.lock();
			threadStatus = this->threadsData[i].status;
			this->threadsData[i].mutex.unlock();

			// Update when thread end generating asked chunks
			if (threadStatus == THREAD_GENERATE_END)
			{
				nbGeneration += this->threadsData[i].maxChunkId.y - this->threadsData[i].minChunkId.y;
				std::cout << "\r  - generate " << nbGeneration << " / " << totalGenerate << std::flush;

				threadStatus = THREAD_RUNNING;
			}

			if (threadStatus == THREAD_RUNNING)
			{
				if (this->currentView.tmpId.x != this->targetView.maxGenChunk.x)
				{
					int	chunkLeftBeforeEndLine = this->targetView.maxGenChunk.y - this->currentView.tmpId.y;

					gm::Vec2i	minId = this->currentView.tmpId;
					this->currentView.tmpId += gm::Vec2i(0, gm::min(chunkLeftBeforeEndLine, heightGeneratePerThread));
					gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(1, 0);

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

					chunkLeftBeforeEndLine = this->targetView.maxGenChunk.y - this->currentView.tmpId.y;
					if (chunkLeftBeforeEndLine == 0)
					{
						this->currentView.tmpId.y = this->targetView.minGenChunk.y;
						this->currentView.tmpId.x++;
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

			status = MAP_MESHING_Y;
		}
	}

	if (status == MAP_MESHING_X)
	{
		bool	allMeshDone = true;
		int		totalWidthMesh = this->targetView.maxMeshChunk.x - this->targetView.minMeshChunk.x;
		int		totalHeightMesh = this->targetView.maxMeshChunk.y - this->targetView.minMeshChunk.y;
		int		totalMesh = totalWidthMesh * totalHeightMesh;
		int		widthMeshPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalWidthMesh / MAP_NB_THREAD);

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

					chunkLeftBeforeEndLine = this->targetView.maxMeshChunk.x - this->currentView.tmpId.x;
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

	else if (status == MAP_MESHING_Y)
	{
		bool	allMeshDone = true;
		int		totalWidthMesh = this->targetView.maxMeshChunk.x - this->targetView.minMeshChunk.x;
		int		totalHeightMesh = this->targetView.maxMeshChunk.y - this->targetView.minMeshChunk.y;
		int		totalMesh = totalWidthMesh * totalHeightMesh;
		int		heightMeshPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalHeightMesh / MAP_NB_THREAD);

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

				nbMesh += maxId.y - minId.y;
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
				if (this->currentView.tmpId.x != this->targetView.maxMeshChunk.x)
				{
					int	chunkLeftBeforeEndLine = this->targetView.maxMeshChunk.y - this->currentView.tmpId.y;

					gm::Vec2i	minId = this->currentView.tmpId;
					this->currentView.tmpId += gm::Vec2i(0, gm::min(chunkLeftBeforeEndLine, heightMeshPerThread));
					gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(1, 0);

					this->threadsData[i].mutex.lock();
					this->threadsData[i].minChunkId = minId;
					this->threadsData[i].maxChunkId = maxId;
					this->threadsData[i].status = THREAD_NEED_MESH;
					this->threadsData[i].mutex.unlock();

					chunkLeftBeforeEndLine = this->targetView.maxMeshChunk.y - this->currentView.tmpId.y;
					if (chunkLeftBeforeEndLine == 0)
					{
						this->currentView.tmpId.y = this->targetView.minMeshChunk.y;
						this->currentView.tmpId.x++;
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

//**** STATIC FUNCTIONS ********************************************************
