#include <program/map/Map.hpp>

#include <unistd.h>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** PUBLIC METHODS **********************************************************

void	Map::update(Engine &engine, Camera &camera)
{
	static MapStatus	status = MAP_NONE;
	static int			nbGeneration = 0;
	static int			nbMesh = 0;
	ThreadStatus		threadStatus;

	if (status == MAP_NONE)
	{
		gm::Vec3f	cameraIdf = camera.getPosition() / (float)CHUNK_SIZE;
		gm::Vec2i	cameraId = gm::Vec2i(cameraIdf.x, cameraIdf.z);
		gm::Vec2i	movement = cameraId - this->cameraChunkId;

		if (cameraId != this->cameraChunkId)
		{
			if (movement.x != 0 && movement.y != 0)
				movement.y = 0;

			this->currentView.minGenChunk = this->cameraChunkId + this->minChunkIdOffset - gm::Vec2i(1, 1);
			this->currentView.maxGenChunk = this->cameraChunkId + this->maxChunkIdOffset + gm::Vec2i(1, 1);
			this->currentView.minMeshChunk = this->cameraChunkId + this->minChunkIdOffset;
			this->currentView.maxMeshChunk = this->cameraChunkId + this->maxChunkIdOffset;
			this->currentView.tmpId = this->currentView.minGenChunk;
			std::cout << "\n\nMovement " << movement << " : need generation !" << std::endl;
			std::cout << "Old target : gen " << this->currentView.minGenChunk << " -> " << this->currentView.maxGenChunk
						<< ", mesh " << this->currentView.minMeshChunk << " -> " << this->currentView.maxMeshChunk <<  std::endl;

			this->cameraChunkId += movement;
			this->targetView.minGenChunk = this->cameraChunkId + this->minChunkIdOffset - gm::Vec2i(1, 1);
			this->targetView.maxGenChunk = this->cameraChunkId + this->maxChunkIdOffset + gm::Vec2i(1, 1);
			this->targetView.minMeshChunk = this->cameraChunkId + this->minChunkIdOffset;
			this->targetView.maxMeshChunk = this->cameraChunkId + this->maxChunkIdOffset;
			this->targetView.tmpId = this->targetView.minGenChunk;

			std::cout << "New target : gen " << this->targetView.minGenChunk << " -> " << this->targetView.maxGenChunk
						<< ", mesh " << this->targetView.minMeshChunk << " -> " << this->targetView.maxMeshChunk <<  std::endl;

			for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
				this->clusters[i].move(*this, movement);
		}

		if (this->currentView == this->targetView)
			return ;

		gm::Vec2i	minDelete = gm::Vec2i(0, 0);
		gm::Vec2i	maxDelete = gm::Vec2i(0, 0);

		// Case of first generation
		if (this->currentView.minGenChunk == this->currentView.maxGenChunk)
		{
			status = MAP_GENERATING_X;
		}
		// Left movement
		else if (movement.x < 0)
		{
			this->targetView.maxGenChunk = gm::Vec2i(this->currentView.minGenChunk.x, this->currentView.maxGenChunk.y);
			this->targetView.maxMeshChunk = gm::Vec2i(this->currentView.minMeshChunk.x, this->currentView.maxMeshChunk.y);
			status = MAP_GENERATING_Y;
			std::cout << "Generate for left : " << this->targetView.minGenChunk << " -> " << this->targetView.maxGenChunk << std::endl;
			std::cout << "Mesh for left     : " << this->targetView.minMeshChunk << " -> " << this->targetView.maxMeshChunk << std::endl;
			std::cout << "Delete for left   : " << minDelete << " -> " << maxDelete << std::endl;
		}
		// Right movement
		else if (movement.x > 0)
		{
			this->targetView.minGenChunk = gm::Vec2i(this->currentView.maxGenChunk.x, this->targetView.minGenChunk.y);
			this->targetView.minMeshChunk = gm::Vec2i(this->currentView.maxMeshChunk.x, this->targetView.minMeshChunk.y);
			status = MAP_GENERATING_Y;
			std::cout << "Generate for right : " << this->targetView.minGenChunk << " -> " << this->targetView.maxGenChunk << std::endl;
			std::cout << "Mesh for right     : " << this->targetView.minMeshChunk << " -> " << this->targetView.maxMeshChunk << std::endl;
			std::cout << "Delete for right   : " << minDelete << " -> " << maxDelete << std::endl;
		}
		// Front movement
		else if (movement.y < 0)
		{
			this->targetView.maxGenChunk = gm::Vec2i(this->targetView.maxGenChunk.x, this->currentView.minGenChunk.y);
			this->targetView.maxMeshChunk = gm::Vec2i(this->targetView.maxMeshChunk.x, this->currentView.minMeshChunk.y);
			status = MAP_GENERATING_X;
			std::cout << "Generate for front : " << this->targetView.minGenChunk << " -> " << this->targetView.maxGenChunk << std::endl;
			std::cout << "Mesh for front     : " << this->targetView.minMeshChunk << " -> " << this->targetView.maxMeshChunk << std::endl;
			std::cout << "Delete for front   : " << minDelete << " -> " << maxDelete << std::endl;
		}
		// Back movement
		else
		{
			this->targetView.minGenChunk = gm::Vec2i(this->targetView.minGenChunk.x, this->currentView.maxMeshChunk.y);
			this->targetView.minMeshChunk = gm::Vec2i(this->targetView.minMeshChunk.x, this->currentView.maxMeshChunk.y);
			status = MAP_GENERATING_X;
			std::cout << "Generate for back : " << this->targetView.minGenChunk << " -> " << this->targetView.maxGenChunk << std::endl;
			std::cout << "Mesh for back     : " << this->targetView.minMeshChunk << " -> " << this->targetView.maxMeshChunk << std::endl;
			std::cout << "Delete for back   : " << minDelete << " -> " << maxDelete << std::endl;
		}

		this->currentView.minGenChunk = this->targetView.minGenChunk;
		this->currentView.maxGenChunk = this->targetView.minGenChunk;
		this->currentView.minMeshChunk = this->targetView.minMeshChunk;
		this->currentView.maxMeshChunk = this->targetView.minMeshChunk;
		this->currentView.tmpId = this->targetView.minGenChunk;
		nbGeneration = 0;
		nbMesh = 0;

		// TODO : Delete useless chunk, update currentView
		std::cout << "\nBegin generation :" << std::endl;
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

					std::size_t	hash;
					for (int x = minId.x; x < maxId.x; x++)
					{
						for (int y = minId.y; y < maxId.y; y++)
						{
							hash = gm::hash(gm::Vec2i(x, y));

							if (this->chunks.find(hash) != this->chunks.end())
								continue;

							this->chunks[hash] = Chunk();
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
