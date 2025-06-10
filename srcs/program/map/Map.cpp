#include <program/map/Map.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Map::Map(void)
{
	this->chunks.resize(CLUSTER_SIZE3);
	for (int i = 0; i < CLUSTER_SIZE3; i++)
		this->chunks[i] = Chunk();
	this->cluster = Cluster();
}


Map::Map(const Map &obj)
{
	this->chunks.resize(CLUSTER_SIZE3);
	for (int i = 0; i < CLUSTER_SIZE3; i++)
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

	for (int i = 0; i < CLUSTER_SIZE3; i++)
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
	gm::Vec3i	chunkId;

	for (int x = 0; x < CLUSTER_SIZE; x++)
	{
		for (int y = 0; y < CLUSTER_SIZE; y++)
		{
			for (int z = 0; z < CLUSTER_SIZE; z++)
			{
				id = x + y * CLUSTER_SIZE + z * CLUSTER_SIZE2;

				chunkId.x = x - halfClusterSize;
				chunkId.y = y;
				chunkId.z = z - halfClusterSize;

				this->chunks[id].init(engine, camera, chunkShader, chunkId);
				this->cluster.chunks[id] = &this->chunks[id];
			}
		}
	}
}


void	Map::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	// this->chunks[0].draw(engine, camera, chunkShader);
	this->cluster.draw(engine, camera, chunkShader);
}


void	Map::destroy(Engine &engine)
{
	for (int i = 0; i < CLUSTER_SIZE3; i++)
		this->chunks[i].destroy(engine);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
