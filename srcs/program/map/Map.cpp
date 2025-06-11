#include <program/map/Map.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Map::Map(void)
{
	this->cluster = Cluster();
}


Map::Map(const Map &obj)
{
	this->chunks = obj.chunks;
	this->cluster = obj.cluster;
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
	this->cluster = obj.cluster;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Map::init(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader)
{
	gm::Vec3i minChunk = gm::Vec2i(-4, -4);
	gm::Vec3i maxChunk = gm::Vec2i(4, 4);
	std::size_t	hash;

	// Generate chunks
	for (int x = minChunk.x; x < maxChunk.x; x++)
	{
		for (int y = minChunk.y; y < maxChunk.y; y++)
		{
			hash = gm::hash(gm::Vec2i(x, y));
			this->chunks[hash] = Chunk();
			this->chunks[hash].init(engine, camera, chunkShader);
			this->chunks[hash].generate(gm::Vec2i(x, y), this->perlin);
		}
	}

	// Create chunks meshes
	for (int x = minChunk.x; x < maxChunk.x; x++)
	{
		for (int y = minChunk.y; y < maxChunk.y; y++)
		{
			hash = gm::hash(gm::Vec2i(x, y));
			this->chunks[hash].updateMeshes(*this);
		}
	}

	this->cluster.setChunks(*this, gm::Vec2i(0, 0));
}


void	Map::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	this->cluster.draw(engine, camera, chunkShader);
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
