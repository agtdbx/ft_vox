#include <program/map/Cluster.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Cluster::Cluster(void)
{
	for (int i = 0; i <  CLUSTER_SIZE3; i++)
		this->chunks[i] = NULL;
}


Cluster::Cluster(const Cluster &obj)
{
	for (int i = 0; i <  CLUSTER_SIZE3; i++)
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

	for (int i = 0; i <  CLUSTER_SIZE3; i++)
		this->chunks[i] = obj.chunks[i];

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Cluster::draw(Engine &engine, Camera &camera, Shader &chunkShader)
{
	for (int i = 0; i <  CLUSTER_SIZE3; i++)
	{
		if (this->chunks[i] != NULL)
			this->chunks[i]->draw(engine, camera, chunkShader);
	}
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
