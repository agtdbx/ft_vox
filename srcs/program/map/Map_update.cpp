#include <program/map/Map.hpp>

#include <unistd.h>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** PUBLIC METHODS **********************************************************

void	Map::update(Engine &engine, Camera &camera)
{
	static MapStatus	status = MAP_NONE;

	if (MAP_NB_THREAD == 0)
		return ;

	if (status == MAP_NONE)
	{
		status = this->prepareGeneration(engine, camera);
	}

	if (status == MAP_GENERATING_X)
	{
		if (this->generatingX())
		{
			this->currentView.maxGenChunk = this->targetView.maxGenChunk;
			this->currentView.tmpId = this->targetView.minMeshChunk;
			status = MAP_MESHING_X;
		}
	}

	else if (status == MAP_GENERATING_Y)
	{
		if (this->generatingY())
		{
			this->currentView.maxGenChunk = this->targetView.maxGenChunk;
			this->currentView.tmpId = this->targetView.minMeshChunk;

			status = MAP_MESHING_Y;
		}
	}

	if (status == MAP_MESHING_X)
	{
		if (this->meshingX())
		{
			this->currentView.maxMeshChunk = this->targetView.maxMeshChunk;
			this->currentView.tmpId = this->minDelete;

			status = MAP_DESTROYING_X;
		}
	}

	else if (status == MAP_MESHING_Y)
	{
		if (this->meshingY())
		{
			this->currentView.maxMeshChunk = this->targetView.maxMeshChunk;
			this->currentView.tmpId = this->minDelete;

			status = MAP_DESTROYING_Y;
		}
	}

	if (status == MAP_DESTROYING_X)
	{
		if (this->destroyingX())
		{
			this->currentView.tmpId = this->targetView.tmpId;

			for (int x = this->minDelete.x; x < this->maxDelete.x; x++)
			{
				for (int y = this->minDelete.y; y < this->maxDelete.y; y++)
				{
					std::size_t	hash = gm::hashSmall(gm::Vec2i(x, y));

					this->chunksMutex.lock();
					if (this->chunks.find(hash) != this->chunks.end())
						this->chunks.erase(hash);
					this->chunksMutex.unlock();
				}
			}

			status = MAP_NONE;
		}
	}

	else if (status == MAP_DESTROYING_Y)
	{
		if (this->destroyingY())
		{
			this->currentView.tmpId = this->targetView.tmpId;

			for (int x = this->minDelete.x; x < this->maxDelete.x; x++)
			{
				for (int y = this->minDelete.y; y < this->maxDelete.y; y++)
				{
					std::size_t	hash = gm::hashSmall(gm::Vec2i(x, y));

					this->chunksMutex.lock();
					if (this->chunks.find(hash) != this->chunks.end())
						this->chunks.erase(hash);
					this->chunksMutex.unlock();
				}
			}

			status = MAP_NONE;
		}
	}
}

//**** PRIVATE METHODS *********************************************************

MapStatus	Map::prepareGeneration(Engine &engine, Camera &camera)
{
	MapStatus	status = MAP_NONE;
	gm::Vec3f	cameraIdf = camera.getPosition() / (float)CHUNK_SIZE;
	gm::Vec2i	cameraId = gm::Vec2i(cameraIdf.x, cameraIdf.z);
	gm::Vec2i	movement = cameraId - this->cameraChunkId;

	if (cameraId != this->cameraChunkId)
	{
		if (movement.x != 0 && movement.y != 0)
		{
			if (movement.x >= movement.y)
				movement.y = 0;
			else
				movement.x = 0;
		}

		this->currentView.minGenChunk = this->cameraChunkId + this->minChunkIdOffset - gm::Vec2i(1, 1);
		this->currentView.maxGenChunk = this->cameraChunkId + this->maxChunkIdOffset + gm::Vec2i(1, 1);
		this->currentView.minMeshChunk = this->cameraChunkId + this->minChunkIdOffset;
		this->currentView.maxMeshChunk = this->cameraChunkId + this->maxChunkIdOffset;
		this->currentView.tmpId = this->currentView.minGenChunk;

		this->cameraChunkId += movement;

		this->targetView.minGenChunk = this->cameraChunkId + this->minChunkIdOffset - gm::Vec2i(1, 1);
		this->targetView.maxGenChunk = this->cameraChunkId + this->maxChunkIdOffset + gm::Vec2i(1, 1);
		this->targetView.minMeshChunk = this->cameraChunkId + this->minChunkIdOffset;
		this->targetView.maxMeshChunk = this->cameraChunkId + this->maxChunkIdOffset;
		this->targetView.tmpId = this->targetView.minGenChunk;

		for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
			this->clusters[i].move(*this, movement);
	}

	if (this->currentView == this->targetView)
	{
		return (status);
	}

	this->minDelete = gm::Vec2i(0, 0);
	this->maxDelete = gm::Vec2i(0, 0);

	// Case of first generation
	if (this->currentView.minGenChunk == this->currentView.maxGenChunk)
	{
		status = MAP_GENERATING_X;
	}
	// Left movement
	else if (movement.x < 0)
	{
		this->minDelete = gm::Vec2i(this->targetView.maxGenChunk.x, this->currentView.minGenChunk.y);
		this->maxDelete = gm::Vec2i(this->currentView.maxGenChunk.x, this->currentView.maxGenChunk.y);
		this->targetView.maxGenChunk = gm::Vec2i(this->currentView.minGenChunk.x, this->currentView.maxGenChunk.y);
		this->targetView.maxMeshChunk = gm::Vec2i(this->currentView.minMeshChunk.x, this->currentView.maxMeshChunk.y);
		status = MAP_GENERATING_Y;
	}
	// Right movement
	else if (movement.x > 0)
	{
		this->minDelete = gm::Vec2i(this->currentView.minGenChunk.x, this->currentView.minGenChunk.y);
		this->maxDelete = gm::Vec2i(this->targetView.minGenChunk.x, this->currentView.maxGenChunk.y);
		this->targetView.minGenChunk = gm::Vec2i(this->currentView.maxGenChunk.x, this->targetView.minGenChunk.y);
		this->targetView.minMeshChunk = gm::Vec2i(this->currentView.maxMeshChunk.x, this->targetView.minMeshChunk.y);
		status = MAP_GENERATING_Y;
	}
	// Front movement
	else if (movement.y < 0)
	{
		this->minDelete = gm::Vec2i(this->currentView.minGenChunk.x, this->targetView.maxGenChunk.y);
		this->maxDelete = gm::Vec2i(this->currentView.maxGenChunk.x, this->currentView.maxGenChunk.y);
		this->targetView.maxGenChunk = gm::Vec2i(this->targetView.maxGenChunk.x, this->currentView.minGenChunk.y);
		this->targetView.maxMeshChunk = gm::Vec2i(this->targetView.maxMeshChunk.x, this->currentView.minMeshChunk.y);
		status = MAP_GENERATING_X;
	}
	// Back movement
	else
	{
		this->minDelete = gm::Vec2i(this->currentView.minGenChunk.x, this->currentView.minGenChunk.y);
		this->maxDelete = gm::Vec2i(this->currentView.maxGenChunk.x, this->targetView.minGenChunk.y);
		this->targetView.minGenChunk = gm::Vec2i(this->targetView.minGenChunk.x, this->currentView.maxMeshChunk.y);
		this->targetView.minMeshChunk = gm::Vec2i(this->targetView.minMeshChunk.x, this->currentView.maxMeshChunk.y);
		status = MAP_GENERATING_X;
	}

	this->currentView.minGenChunk = this->targetView.minGenChunk;
	this->currentView.maxGenChunk = this->targetView.minGenChunk;
	this->currentView.minMeshChunk = this->targetView.minMeshChunk;
	this->currentView.maxMeshChunk = this->targetView.minMeshChunk;
	this->currentView.tmpId = this->targetView.minGenChunk;

	return (status);
}


bool	Map::generatingX(void)
{
	bool			allGenerationDone = true;
	int				totalWidthGenerate = this->targetView.maxGenChunk.x - this->targetView.minGenChunk.x;
	int				widthGeneratePerThread = gm::max(MIN_CHUNK_PER_THREAD, totalWidthGenerate / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int i = 0; i < MAP_NB_THREAD; i++)
	{
		this->threadsData[i].mutex.lock();
		threadStatus = this->threadsData[i].status;
		this->threadsData[i].mutex.unlock();

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
						hash = gm::hashSmall(gm::Vec2i(x, y));

						this->chunksMutex.lock();
						ChunkMap::iterator	it = this->chunks.find(hash);
						this->chunksMutex.unlock();

						if (it != this->chunks.end())
							continue;

						try
						{
							this->chunksMutex.lock();
							this->chunks.insert(std::pair<std::size_t, Chunk>(hash, Chunk()));
							this->chunksMutex.unlock();
						}
						catch(const std::exception& e)
						{
						}
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
		}
		else
		{
			allGenerationDone = false;
		}
	}

	return (allGenerationDone);
}


bool	Map::generatingY(void)
{
	bool			allGenerationDone = true;
	int				totalHeightGenerate = this->targetView.maxGenChunk.y - this->targetView.minGenChunk.y;
	int				heightGeneratePerThread = gm::max(MIN_CHUNK_PER_THREAD, totalHeightGenerate / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int i = 0; i < MAP_NB_THREAD; i++)
	{
		this->threadsData[i].mutex.lock();
		threadStatus = this->threadsData[i].status;
		this->threadsData[i].mutex.unlock();

		if (threadStatus == THREAD_RUNNING)
		{
			if (this->currentView.tmpId.x != this->targetView.maxGenChunk.x)
			{
				int	chunkLeftBeforeEndLine = this->targetView.maxGenChunk.y - this->currentView.tmpId.y;

				gm::Vec2i	minId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(0, gm::min(chunkLeftBeforeEndLine, heightGeneratePerThread));
				gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(1, 0);

				std::size_t	hash;
				for (int x = minId.x; x < maxId.x; x++)
				{
					for (int y = minId.y; y < maxId.y; y++)
					{
						hash = gm::hashSmall(gm::Vec2i(x, y));

						this->chunksMutex.lock();
						ChunkMap::iterator	it = this->chunks.find(hash);
						this->chunksMutex.unlock();

						if (it != this->chunks.end())
							continue;

						try
						{
							this->chunksMutex.lock();
							this->chunks.insert(std::pair<std::size_t, Chunk>(hash, Chunk()));
							this->chunksMutex.unlock();
						}
						catch(const std::exception& e)
						{
						}
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
		}
		else
			allGenerationDone = false;
	}

	return (allGenerationDone);
}


bool	Map::meshingX(void)
{
	bool			allMeshDone = true;
	int				totalWidthMesh = this->targetView.maxMeshChunk.x - this->targetView.minMeshChunk.x;
	int				widthMeshPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalWidthMesh / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

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

			for (int x = minId.x; x < maxId.x; x++)
			{
				for (int y = minId.y; y < maxId.y; y++)
				{
					gm::Vec2i	chunkPos = gm::Vec2i(x, y);

					this->chunksMutex.lock();
					ChunkMap::iterator	it = this->chunks.find(gm::hashSmall(chunkPos));
					this->chunksMutex.unlock();

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

	return (allMeshDone);
}


bool	Map::meshingY(void)
{
	bool			allMeshDone = true;
	int				totalHeightMesh = this->targetView.maxMeshChunk.y - this->targetView.minMeshChunk.y;
	int				heightMeshPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalHeightMesh / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

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

			for (int x = minId.x; x < maxId.x; x++)
			{
				for (int y = minId.y; y < maxId.y; y++)
				{
					gm::Vec2i	chunkPos = gm::Vec2i(x, y);
					this->chunksMutex.lock();
					ChunkMap::iterator	it = this->chunks.find(gm::hashSmall(chunkPos));
					this->chunksMutex.unlock();

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

	return (allMeshDone);
}


bool	Map::destroyingX(void)
{
	bool			allDestroyDone = true;
	int				totalWidthDestroy = this->maxDelete.x - this->minDelete.x;
	int				widthDestroyPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalWidthDestroy / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int i = 0; i < MAP_NB_THREAD; i++)
	{
		this->threadsData[i].mutex.lock();
		threadStatus = this->threadsData[i].status;
		this->threadsData[i].mutex.unlock();

		if (threadStatus == THREAD_RUNNING)
		{
			if (this->currentView.tmpId.y != this->maxDelete.y)
			{
				int	chunkLeftBeforeEndLine = this->maxDelete.x - this->currentView.tmpId.x;

				gm::Vec2i	minId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(gm::min(chunkLeftBeforeEndLine, widthDestroyPerThread), 0);
				gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(0, 1);

				this->threadsData[i].mutex.lock();
				this->threadsData[i].minChunkId = minId;
				this->threadsData[i].maxChunkId = maxId;
				this->threadsData[i].status = THREAD_NEED_DESTROY;
				this->threadsData[i].mutex.unlock();

				for (int x = minId.x; x < maxId.x; x++)
				{
					for (int y = minId.y; y < maxId.y; y++)
					{
						this->chunksMutex.lock();
						ChunkMap::iterator it = this->chunks.find(gm::hashSmall(gm::Vec2i(x, y)));
						this->chunksMutex.unlock();

						if (it == this->chunks.end())
							continue;

						it->second.setDrawable(false);
					}
				}

				chunkLeftBeforeEndLine = this->maxDelete.x - this->currentView.tmpId.x;
				if (chunkLeftBeforeEndLine == 0)
				{
					this->currentView.tmpId.x = this->minDelete.x;
					this->currentView.tmpId.y++;
				}

				allDestroyDone = false;
			}
		}
		else
			allDestroyDone = false;
	}

	return (allDestroyDone);
}


bool	Map::destroyingY(void)
{
	bool			allDestroyDone = true;
	int				totalHeightDestroy = this->maxDelete.y - this->minDelete.y;
	int				heightDestroyPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalHeightDestroy / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int i = 0; i < MAP_NB_THREAD; i++)
	{
		this->threadsData[i].mutex.lock();
		threadStatus = this->threadsData[i].status;
		this->threadsData[i].mutex.unlock();

		if (threadStatus == THREAD_RUNNING)
		{
			if (this->currentView.tmpId.x != this->maxDelete.x)
			{
				int	chunkLeftBeforeEndLine = this->maxDelete.y - this->currentView.tmpId.y;

				gm::Vec2i	minId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(0, gm::min(chunkLeftBeforeEndLine, heightDestroyPerThread));
				gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(1, 0);

				this->threadsData[i].mutex.lock();
				this->threadsData[i].minChunkId = minId;
				this->threadsData[i].maxChunkId = maxId;
				this->threadsData[i].status = THREAD_NEED_DESTROY;
				this->threadsData[i].mutex.unlock();

				for (int x = minId.x; x < maxId.x; x++)
				{
					for (int y = minId.y; y < maxId.y; y++)
					{
						this->chunksMutex.lock();
						ChunkMap::iterator it = this->chunks.find(gm::hashSmall(gm::Vec2i(x, y)));
						this->chunksMutex.unlock();

						if (it == this->chunks.end())
							continue;

						it->second.setDrawable(false);
					}
				}

				chunkLeftBeforeEndLine = this->maxDelete.y - this->currentView.tmpId.y;
				if (chunkLeftBeforeEndLine == 0)
				{
					this->currentView.tmpId.y = this->minDelete.y;
					this->currentView.tmpId.x++;
				}

				allDestroyDone = false;
			}
		}
		else
			allDestroyDone = false;
	}

	return (allDestroyDone);
}


bool	Map::destroyingChunks(void)
{
	bool			allDestroyDone = true;
	int				totalWidthDestroy = this->maxDelete.x - this->minDelete.x;
	int				widthDestroyPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalWidthDestroy / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int i = 0; i < MAP_NB_THREAD; i++)
	{
		this->threadsData[i].mutex.lock();
		threadStatus = this->threadsData[i].status;
		this->threadsData[i].mutex.unlock();

		// Update when thread end destroying asked chunks
		if (threadStatus == THREAD_MESH_END)
			threadStatus = THREAD_RUNNING;

		if (threadStatus == THREAD_RUNNING)
		{
			if (this->currentView.tmpId.y != this->maxDelete.y)
			{
				int	chunkLeftBeforeEndLine = this->maxDelete.x - this->currentView.tmpId.x;

				gm::Vec2i	minId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(gm::min(chunkLeftBeforeEndLine, widthDestroyPerThread), 0);
				gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(0, 1);

				this->threadsData[i].mutex.lock();
				this->threadsData[i].minChunkId = minId;
				this->threadsData[i].maxChunkId = maxId;
				this->threadsData[i].status = THREAD_NEED_DESTROY;
				this->threadsData[i].mutex.unlock();

				chunkLeftBeforeEndLine = this->maxDelete.x - this->currentView.tmpId.x;
				if (chunkLeftBeforeEndLine == 0)
				{
					this->currentView.tmpId.x = this->minDelete.x;
					this->currentView.tmpId.y++;
				}

				allDestroyDone = false;
			}
			else
			{
				this->threadsData[i].mutex.lock();
				this->threadsData[i].status = THREAD_RUNNING;
				this->threadsData[i].mutex.unlock();
			}
		}
		else
			allDestroyDone = false;
	}

	return (allDestroyDone);
}

//**** STATIC FUNCTIONS ********************************************************
