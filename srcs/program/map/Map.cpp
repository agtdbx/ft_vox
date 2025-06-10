#include <program/map/Map.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Map::Map(void)
{
	this->chunks.resize(CLUSTER_SIZE2);
	for (int i = 0; i < CLUSTER_SIZE2; i++)
		this->chunks[i] = Chunk();
	this->cluster = Cluster();
}


Map::Map(const Map &obj)
{
	this->chunks.resize(CLUSTER_SIZE2);
	for (int i = 0; i < CLUSTER_SIZE2; i++)
		this->chunks[i] = obj.chunks[i];
	this->cluster = obj.cluster;
}

//---- Destructor --------------------------------------------------------------

Map::~Map()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

Map	&Map::operator=(const Map &obj)
{
	if (this == &obj)
		return (*this);

	for (int i = 0; i < CLUSTER_SIZE2; i++)
		this->chunks[i] = obj.chunks[i];
	this->cluster = obj.cluster;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Map::init(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader)
{
	int	id;
	int	halfClusterSize = CLUSTER_SIZE / 2;
	gm::Vec2i	chunkId;

	// Generate chunks
	for (int x = 0; x < CLUSTER_SIZE; x++)
	{
		for (int y = 0; y < CLUSTER_SIZE; y++)
		{
			id = x + y * CLUSTER_SIZE;

			chunkId.x = x - halfClusterSize;
			chunkId.y = y - halfClusterSize;

			this->chunks[id].init(engine, camera, chunkShader);
			this->chunks[id].generate(chunkId);
			this->cluster.chunks[id] = &this->chunks[id];
		}
	}

	// Create chunks meshes
	for (int i = 0; i < CLUSTER_SIZE2; i++)
		this->chunks[i].updateMeshes();
}


void	Map::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	for (int i = 0; i < CLUSTER_SIZE2; i++)
		this->chunks[i].draw(engine, camera, chunkShader);
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
