#include <program/map/Map.hpp>

#include <unistd.h>
#include <chrono>

//**** STATIC VARIABLES DEFINE *************************************************
//**** STATIC FUNCTIONS DEFINE *************************************************
//**** PUBLIC METHODS **********************************************************

void	Map::update(Engine &engine, Camera &camera)
{
	if (MAP_NB_THREAD == 0)
		return ;

	if (this->status == MAP_NONE)
		this->prepareGeneration(engine, camera);

	if (this->status == MAP_NONE)
		return ;

	engine.chunkFreeableMutex.lock();
	engine.chunkFreeable = MAX_CHUNKS_FREE_PER_LOOP;
	engine.chunkFreeableMutex.unlock();

	if (this->status == MAP_GENERATING_X)
		this->generatingX();
	else if (this->status == MAP_GENERATING_Y)
		this->generatingY();
	else if (this->status == MAP_MESHING_X)
			this->meshingX();
	else if (this->status == MAP_MESHING_Y)
			this->meshingY();
	else if (this->status == MAP_DESTROYING_X)
		this->destroyingX();
	else if (this->status == MAP_DESTROYING_Y)
		this->destroyingY();
}

//**** PRIVATE METHODS *********************************************************

void	Map::prepareGeneration(Engine &engine, Camera &camera)
{
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

		this->clustersMutex.lock();
		for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
			this->clusters[i].move(*this, movement);
		this->clustersMutex.unlock();
	}

	if (this->currentView == this->targetView)
		return ;

	this->minDelete = gm::Vec2i(0, 0);
	this->maxDelete = gm::Vec2i(0, 0);

	// Case of first generation
	if (this->currentView.minGenChunk == this->currentView.maxGenChunk)
	{
		this->status = MAP_GENERATING_X;
	}
	// Left movement
	else if (movement.x < 0)
	{
		this->minDelete = gm::Vec2i(this->targetView.maxGenChunk.x, this->currentView.minGenChunk.y);
		this->maxDelete = gm::Vec2i(this->currentView.maxGenChunk.x, this->currentView.maxGenChunk.y);
		this->targetView.maxGenChunk = gm::Vec2i(this->currentView.minGenChunk.x, this->currentView.maxGenChunk.y);
		this->targetView.maxMeshChunk = gm::Vec2i(this->currentView.minMeshChunk.x, this->currentView.maxMeshChunk.y);
		this->status = MAP_GENERATING_Y;
	}
	// Right movement
	else if (movement.x > 0)
	{
		this->minDelete = gm::Vec2i(this->currentView.minGenChunk.x, this->currentView.minGenChunk.y);
		this->maxDelete = gm::Vec2i(this->targetView.minGenChunk.x, this->currentView.maxGenChunk.y);
		this->targetView.minGenChunk = gm::Vec2i(this->currentView.maxGenChunk.x, this->targetView.minGenChunk.y);
		this->targetView.minMeshChunk = gm::Vec2i(this->currentView.maxMeshChunk.x, this->targetView.minMeshChunk.y);
		this->status = MAP_GENERATING_Y;
	}
	// Front movement
	else if (movement.y < 0)
	{
		this->minDelete = gm::Vec2i(this->currentView.minGenChunk.x, this->targetView.maxGenChunk.y);
		this->maxDelete = gm::Vec2i(this->currentView.maxGenChunk.x, this->currentView.maxGenChunk.y);
		this->targetView.maxGenChunk = gm::Vec2i(this->targetView.maxGenChunk.x, this->currentView.minGenChunk.y);
		this->targetView.maxMeshChunk = gm::Vec2i(this->targetView.maxMeshChunk.x, this->currentView.minMeshChunk.y);
		this->status = MAP_GENERATING_X;
	}
	// Back movement
	else
	{
		this->minDelete = gm::Vec2i(this->currentView.minGenChunk.x, this->currentView.minGenChunk.y);
		this->maxDelete = gm::Vec2i(this->currentView.maxGenChunk.x, this->targetView.minGenChunk.y);
		this->targetView.minGenChunk = gm::Vec2i(this->targetView.minGenChunk.x, this->currentView.maxMeshChunk.y);
		this->targetView.minMeshChunk = gm::Vec2i(this->targetView.minMeshChunk.x, this->currentView.maxMeshChunk.y);
		this->status = MAP_GENERATING_X;
	}

	this->currentView.minGenChunk = this->targetView.minGenChunk;
	this->currentView.maxGenChunk = this->targetView.minGenChunk;
	this->currentView.minMeshChunk = this->targetView.minMeshChunk;
	this->currentView.maxMeshChunk = this->targetView.minMeshChunk;
	this->currentView.tmpId = this->targetView.minGenChunk;
}


void	Map::generatingX(void)
{
	bool			allGenerationDone = true;
	int				totalWidthGenerate = this->targetView.maxGenChunk.x - this->targetView.minGenChunk.x;
	int				widthGeneratePerThread = gm::max(MIN_CHUNK_PER_THREAD, totalWidthGenerate / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int threadId = 0; threadId < MAP_NB_THREAD; threadId++)
	{
		this->threadsData[threadId].mutex.lock();
		threadStatus = this->threadsData[threadId].status;
		this->threadsData[threadId].mutex.unlock();

		if (threadStatus == THREAD_RUNNING)
		{
			if (this->currentView.tmpId.y != this->targetView.maxGenChunk.y)
			{
				int	chunkLeftBeforeEndLine = this->targetView.maxGenChunk.x - this->currentView.tmpId.x;

				gm::Vec2i	minId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(gm::min(chunkLeftBeforeEndLine, widthGeneratePerThread), 0);
				gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(0, 1);

				this->threadsData[threadId].mutex.lock();
				this->threadsData[threadId].minChunkId = minId;
				this->threadsData[threadId].maxChunkId = maxId;
				this->threadsData[threadId].status = THREAD_NEED_GENERATE;
				this->threadsData[threadId].mutex.unlock();

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
			allGenerationDone = false;
	}

	if (allGenerationDone)
	{
		this->status = MAP_MESHING_X;
		this->currentView.maxGenChunk = this->targetView.maxGenChunk;
		this->currentView.tmpId = this->targetView.minMeshChunk;
	}
}


void	Map::generatingY(void)
{
	bool			allGenerationDone = true;
	int				totalHeightGenerate = this->targetView.maxGenChunk.y - this->targetView.minGenChunk.y;
	int				heightGeneratePerThread = gm::max(MIN_CHUNK_PER_THREAD, totalHeightGenerate / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int threadId = 0; threadId < MAP_NB_THREAD; threadId++)
	{
		this->threadsData[threadId].mutex.lock();
		threadStatus = this->threadsData[threadId].status;
		this->threadsData[threadId].mutex.unlock();

		if (threadStatus == THREAD_RUNNING)
		{
			if (this->currentView.tmpId.x != this->targetView.maxGenChunk.x)
			{
				int	chunkLeftBeforeEndLine = this->targetView.maxGenChunk.y - this->currentView.tmpId.y;

				gm::Vec2i	minId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(0, gm::min(chunkLeftBeforeEndLine, heightGeneratePerThread));
				gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(1, 0);

				this->threadsData[threadId].mutex.lock();
				this->threadsData[threadId].minChunkId = minId;
				this->threadsData[threadId].maxChunkId = maxId;
				this->threadsData[threadId].status = THREAD_NEED_GENERATE;
				this->threadsData[threadId].mutex.unlock();

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

	if (allGenerationDone)
	{
		this->status = MAP_MESHING_Y;
		this->currentView.maxGenChunk = this->targetView.maxGenChunk;
		this->currentView.tmpId = this->targetView.minMeshChunk;
	}
}


void	Map::meshingX(void)
{
	bool			allMeshDone = true;
	int				totalWidthMesh = this->targetView.maxMeshChunk.x - this->targetView.minMeshChunk.x;
	int				widthMeshPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalWidthMesh / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int threadId = 0; threadId < MAP_NB_THREAD; threadId++)
	{
		this->threadsData[threadId].mutex.lock();
		threadStatus = this->threadsData[threadId].status;
		this->threadsData[threadId].mutex.unlock();

		if (threadStatus == THREAD_RUNNING)
		{
			if (this->currentView.tmpId.y != this->targetView.maxMeshChunk.y)
			{
				int	chunkLeftBeforeEndLine = this->targetView.maxMeshChunk.x - this->currentView.tmpId.x;

				gm::Vec2i	minId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(gm::min(chunkLeftBeforeEndLine, widthMeshPerThread), 0);
				gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(0, 1);

				this->threadsData[threadId].mutex.lock();
				this->threadsData[threadId].minChunkId = minId;
				this->threadsData[threadId].maxChunkId = maxId;
				this->threadsData[threadId].status = THREAD_NEED_MESH;
				this->threadsData[threadId].mutex.unlock();

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
				this->threadsData[threadId].mutex.lock();
				this->threadsData[threadId].status = THREAD_RUNNING;
				this->threadsData[threadId].mutex.unlock();
			}
		}
		else
			allMeshDone = false;
	}

	if (allMeshDone)
	{
		this->status = MAP_DESTROYING_X;
		this->currentView.maxMeshChunk = this->targetView.maxMeshChunk;
		this->currentView.tmpId = this->minDelete;
	}
}


void	Map::meshingY(void)
{
	bool			allMeshDone = true;
	int				totalHeightMesh = this->targetView.maxMeshChunk.y - this->targetView.minMeshChunk.y;
	int				heightMeshPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalHeightMesh / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int threadId = 0; threadId < MAP_NB_THREAD; threadId++)
	{
		this->threadsData[threadId].mutex.lock();
		threadStatus = this->threadsData[threadId].status;
		this->threadsData[threadId].mutex.unlock();

		if (threadStatus == THREAD_RUNNING)
		{
			if (this->currentView.tmpId.x != this->targetView.maxMeshChunk.x)
			{
				int	chunkLeftBeforeEndLine = this->targetView.maxMeshChunk.y - this->currentView.tmpId.y;

				gm::Vec2i	minId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(0, gm::min(chunkLeftBeforeEndLine, heightMeshPerThread));
				gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(1, 0);

				this->threadsData[threadId].mutex.lock();
				this->threadsData[threadId].minChunkId = minId;
				this->threadsData[threadId].maxChunkId = maxId;
				this->threadsData[threadId].status = THREAD_NEED_MESH;
				this->threadsData[threadId].mutex.unlock();

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
				this->threadsData[threadId].mutex.lock();
				this->threadsData[threadId].status = THREAD_RUNNING;
				this->threadsData[threadId].mutex.unlock();
			}
		}
		else
			allMeshDone = false;
	}

	if (allMeshDone)
	{
		this->status = MAP_DESTROYING_Y;
		this->currentView.maxMeshChunk = this->targetView.maxMeshChunk;
		this->currentView.tmpId = this->minDelete;
	}
}


void	Map::destroyingX(void)
{
	bool			allDestroyDone = true;
	int				totalWidthDestroy = this->maxDelete.x - this->minDelete.x;
	int				widthDestroyPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalWidthDestroy / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int threadId = 0; threadId < MAP_NB_THREAD; threadId++)
	{
		this->threadsData[threadId].mutex.lock();
		threadStatus = this->threadsData[threadId].status;
		this->threadsData[threadId].mutex.unlock();

		if (threadStatus == THREAD_RUNNING)
		{
			if (this->currentView.tmpId.y != this->maxDelete.y)
			{
				int	chunkLeftBeforeEndLine = this->maxDelete.x - this->currentView.tmpId.x;

				gm::Vec2i	minId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(gm::min(chunkLeftBeforeEndLine, widthDestroyPerThread), 0);
				gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(0, 1);

				this->threadsData[threadId].mutex.lock();
				this->threadsData[threadId].minChunkId = minId;
				this->threadsData[threadId].maxChunkId = maxId;
				this->threadsData[threadId].status = THREAD_NEED_DESTROY;
				this->threadsData[threadId].mutex.unlock();

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

	if (allDestroyDone)
	{
		this->status = MAP_NONE;
		this->currentView.tmpId = this->targetView.tmpId;
	}
}


void	Map::destroyingY(void)
{
	bool			allDestroyDone = true;
	int				totalHeightDestroy = this->maxDelete.y - this->minDelete.y;
	int				heightDestroyPerThread = gm::max(MIN_CHUNK_PER_THREAD, totalHeightDestroy / MAP_NB_THREAD);
	ThreadStatus	threadStatus;

	for (int threadId = 0; threadId < MAP_NB_THREAD; threadId++)
	{
		this->threadsData[threadId].mutex.lock();
		threadStatus = this->threadsData[threadId].status;
		this->threadsData[threadId].mutex.unlock();

		if (threadStatus == THREAD_RUNNING)
		{
			if (this->currentView.tmpId.x != this->maxDelete.x)
			{
				int	chunkLeftBeforeEndLine = this->maxDelete.y - this->currentView.tmpId.y;

				gm::Vec2i	minId = this->currentView.tmpId;
				this->currentView.tmpId += gm::Vec2i(0, gm::min(chunkLeftBeforeEndLine, heightDestroyPerThread));
				gm::Vec2i	maxId = this->currentView.tmpId + gm::Vec2i(1, 0);

				this->threadsData[threadId].mutex.lock();
				this->threadsData[threadId].minChunkId = minId;
				this->threadsData[threadId].maxChunkId = maxId;
				this->threadsData[threadId].status = THREAD_NEED_DESTROY;
				this->threadsData[threadId].mutex.unlock();

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

	if (allDestroyDone)
	{
		this->status = MAP_NONE;
		this->currentView.tmpId = this->targetView.tmpId;
	}
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
				this->threadsData[i].status = THREAD_NEED_FINAL_DESTROY;
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
