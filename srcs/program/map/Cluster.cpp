#include <program/map/Cluster.hpp>

#include <program/map/Map.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Cluster::Cluster(void)
{
	for (int i = 0; i <  CLUSTER_SIZE2; i++)
		this->chunks[i] = NULL;

	int	halfClusterSize = (CLUSTER_SIZE / 2) * CHUNK_SIZE;
	int	halfClusterHeight = CHUNK_HEIGHT / 2;
	this->boundingCube.center = gm::Vec3f(0, 0, 0);
	this->boundingCube.extents = gm::Vec3f(halfClusterSize, halfClusterHeight, halfClusterSize);
}


Cluster::Cluster(const Cluster &obj)
{
	for (int i = 0; i <  CLUSTER_SIZE2; i++)
		this->chunks[i] = obj.chunks[i];

	this->boundingCube = obj.boundingCube;
}

//---- Destructor --------------------------------------------------------------

Cluster::~Cluster()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

const BoundingCube	&Cluster::getBoundingCube(void) const
{
	return (this->boundingCube);
}

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

Cluster	&Cluster::operator=(const Cluster &obj)
{
	if (this == &obj)
		return (*this);

	for (int i = 0; i <  CLUSTER_SIZE2; i++)
		this->chunks[i] = obj.chunks[i];

	this->boundingCube = obj.boundingCube;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Cluster::setPosition(const gm::Vec2i &middle)
{
	this->minChunk = middle - gm::Vec2i(CLUSTER_SIZE / 2, CLUSTER_SIZE / 2);
	this->maxChunk = middle + gm::Vec2i(CLUSTER_SIZE / 2, CLUSTER_SIZE / 2);

	this->boundingCube.center = gm::Vec3f(middle.x, 0, middle.y) * (float)CHUNK_SIZE;
	this->boundingCube.computePoints();
}


void	Cluster::giveChunk(const gm::Vec2i &chunkPos, Chunk *chunk)
{
	if (chunkPos.x < this->minChunk.x || chunkPos.y < this->minChunk.y
		|| chunkPos.x >= this->maxChunk.x || chunkPos.y >= this->maxChunk.y)
		return ;

	int	cx = chunkPos.x - this->minChunk.x;
	int	cy = chunkPos.y - this->minChunk.y;
	int	id = cx + cy * CLUSTER_SIZE;

	this->chunks[id] = chunk;
}


void	Cluster::draw(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader,
				int &nbDrawCall)
{
	for (int i = 0; i <  CLUSTER_SIZE2; i++)
	{
		if (this->chunks[i] != NULL)
		{
			if (camera.isCubeInFrutum(this->chunks[i]->getBoundingCube()))
			{
				this->chunks[i]->draw(engine, camera, chunkShader);
				nbDrawCall++;
			}
		}
	}
}


void	Cluster::drawWater(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader)
{
	for (int i = 0; i <  CLUSTER_SIZE2; i++)
	{
		if (this->chunks[i] != NULL)
		{
			if (camera.isCubeInFrutum(this->chunks[i]->getBoundingCube()))
				this->chunks[i]->drawWater(engine, camera, chunkShader);
		}
	}
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
