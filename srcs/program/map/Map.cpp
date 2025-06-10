#include <program/map/Map.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Map::Map(void)
{
	for (int i = 0; i < NB_CHUNK; i++)
		this->chunks[i] = Chunk();
	this->cluster = Cluster();
}


Map::Map(const Map &obj)
{
	for (int i = 0; i < NB_CHUNK; i++)
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

	for (int i = 0; i < NB_CHUNK; i++)
		this->chunks[i] = obj.chunks[i];
	this->cluster = obj.cluster;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Map::init(VulkanCommandPool &commandPool, Camera &camera)
{
	// Init chunks
	std::vector<gm::Vec3f>	chunksPos = {
		{  0,  0,  0},
		{ 32,  0,  0},
		{-32,  0,  0},
		{  0,  0, 32},
		{ 32,  0, 32},
		{-32,  0, 32},
		{  0,  0,-32},
		{ 32,  0,-32},
		{-32,  0,-32},
	};

	for (int i = 0; i < NB_CHUNK; i++)
		this->chunks[i].init(commandPool, camera, chunksPos[i]);

	// Init cluster
	for (int i = 0; i < NB_CHUNK; i++)
		this->cluster.chunks[i] = &this->chunks[i];
}


void	Map::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	// this->chunks[0].draw(engine, camera, chunkShader);
	this->cluster.draw(engine, camera, chunkShader);
}


void	Map::destroy(void)
{
	for (int i = 0; i < NB_CHUNK; i++)
		this->chunks[i].destroy();
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
