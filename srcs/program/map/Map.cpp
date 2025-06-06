#include <program/map/Map.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Map::Map(void)
{
	this->chunk = Chunk();
}


Map::Map(const Map &obj)
{
	this->chunk = obj.chunk;
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

	this->chunk = obj.chunk;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Map::init(VulkanCommandPool &commandPool, Camera &camera)
{
	this->chunk.init(commandPool, camera);
}


void	Map::draw(Engine &engine, Camera &camera, Shader &chunkShader)
{
	this->chunk.draw(engine, camera, chunkShader);
}


void	Map::destroy(void)
{
	this->chunk.destroy();
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
