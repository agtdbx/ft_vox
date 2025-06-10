#include <program/map/Cluster.hpp>

#include <program/map/Map.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Cluster::Cluster(void)
{
	for (int i = 0; i <  CLUSTER_SIZE2; i++)
		this->chunks[i] = NULL;
}


Cluster::Cluster(const Cluster &obj)
{
	for (int i = 0; i <  CLUSTER_SIZE2; i++)
		this->chunks[i] = obj.chunks[i];
}

//---- Destructor --------------------------------------------------------------

Cluster::~Cluster()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

Cluster	&Cluster::operator=(const Cluster &obj)
{
	if (this == &obj)
		return (*this);

	for (int i = 0; i <  CLUSTER_SIZE2; i++)
		this->chunks[i] = obj.chunks[i];

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Cluster::setChunks(Map &map, const gm::Vec2i &middle)
{
	int	cx, cy, id;

	this->minChunk = middle - gm::Vec2i(CLUSTER_SIZE / 2, CLUSTER_SIZE / 2);
	this->maxChunk = middle + gm::Vec2i(CLUSTER_SIZE / 2, CLUSTER_SIZE / 2);

	for (int x = this->minChunk.x; x < this->maxChunk.x; x++)
	{
		cx = x - this->minChunk.x;
		for (int y = this->minChunk.y; y < this->maxChunk.y; y++)
		{
			cy = y - this->minChunk.y;
			id = cx + cy * CLUSTER_SIZE;
			this->chunks[id] = map.getChunk(x, y);
		}
	}
}


void	Cluster::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	for (int i = 0; i <  CLUSTER_SIZE2; i++)
	{
		if (this->chunks[i] != NULL)
			this->chunks[i]->draw(engine, camera, chunkShader);
	}
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
