#include <program/map/Map.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Map::Map(void)
{
}


Map::Map(const Map &obj)
{
	this->chunks = obj.chunks;
	this->clusters = obj.clusters;
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
	for (int i = 0; i < MAP_CLUSTER_SIZE; i++)
		this->clusters[i] = Cluster();

	int	nbChunkArround = CLUSTER_SIZE / 2 + MAP_CLUSTER_ARROUND * CLUSTER_SIZE;
	gm::Vec2i minChunk = gm::Vec2i(-nbChunkArround, -nbChunkArround);
	gm::Vec2i maxChunk = gm::Vec2i( nbChunkArround,  nbChunkArround);
	std::size_t	hash;

	PerfLogger	perfLogger;
	resetLog(perfLogger.generation);
	resetLog(perfLogger.createMesh);
	perfLogger.nbTriangles = 0;

	// Generate chunks
	int	nbChunks = 0;
	int	totalChunks = (nbChunkArround * 2) * (nbChunkArround * 2);
	printf("Chunks generation :\n");
	for (int x = minChunk.x; x < maxChunk.x; x++)
	{
		for (int y = minChunk.y; y < maxChunk.y; y++)
		{
			nbChunks++;
			printf("\r chunk %i/%i", nbChunks, totalChunks);

			hash = gm::hash(gm::Vec2i(x, y));
			this->chunks[hash] = Chunk();
			this->chunks[hash].init(engine, camera, chunkShader);
			this->chunks[hash].generate(gm::Vec2i(x, y), perfLogger);
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
			this->chunks[hash].updateMeshes(*this, perfLogger);
		}
	}
	printf("\n\n");

	printLog(perfLogger.generation, "Chunk generation");
	printLog(perfLogger.createMesh, "Mesh creation");
	printf("Number of triangle : %i\n", perfLogger.nbTriangles);

	int	minClusterId = -MAP_CLUSTER_ARROUND * CLUSTER_SIZE;
	int	idY, chunkX, chunkY;
	for (int y = 0; y < MAP_CLUSTER_WIDTH; y++)
	{
		chunkY = minClusterId + y * CLUSTER_SIZE;
		idY = y * MAP_CLUSTER_WIDTH;
		for (int x = 0; x < MAP_CLUSTER_WIDTH; x++)
		{
			chunkX = minClusterId + x * CLUSTER_SIZE;
			this->clusters[x + idY].setChunks(*this, gm::Vec2i(chunkX, chunkY));
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
	for (int i = 0; i < CLUSTER_SIZE2; i++)
		this->chunks[i].destroy(engine);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
