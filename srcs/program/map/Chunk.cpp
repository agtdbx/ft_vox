#include <program/map/Chunk.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Chunk::Chunk(void)
{
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				int	id = x + y * CHUNK_SIZE + z * CHUNK_SIZE2;
				if (y > 8)
					this->cubes[id] = CUBE_AIR;
				else
					this->cubes[id] = CUBE_DIRT;
			}
		}
	}
	this->copyCommandPool = NULL;
}


Chunk::Chunk(const Chunk &obj)
{
	for (int i = 0; i < CHUNK_SIZE3; i++)
		this->cubes[i] = obj.cubes[i];
	this->copyCommandPool = obj.copyCommandPool;
}

//---- Destructor --------------------------------------------------------------

Chunk::~Chunk()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

Cube	Chunk::getCube(unsigned int x, unsigned int y, unsigned int z)
{
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE)
		return (CUBE_AIR);
	return (this->cubes[x + y * CHUNK_SIZE + z * CHUNK_SIZE2]);
}

//---- Setters -----------------------------------------------------------------

void	Chunk::setCube(unsigned int x, unsigned int y, unsigned int z, Cube cube)
{
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE)
		return ;
	this->cubes[x + y * CHUNK_SIZE + z * CHUNK_SIZE2] = cube;
}

//---- Operators ---------------------------------------------------------------

Chunk	&Chunk::operator=(const Chunk &obj)
{
	if (this == &obj)
		return (*this);

	for (int i = 0; i < CHUNK_SIZE3; i++)
		this->cubes[i] = obj.cubes[i];

	if (!this->copyCommandPool)
		this->copyCommandPool = obj.copyCommandPool;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Chunk::init(VulkanCommandPool &commandPool, Camera &camera)
{
	this->copyCommandPool = &commandPool;
	this->generateMeshes();
	this->updateMeshes();

	this->uboUp.proj = camera.getProjection();
	this->uboUp.proj.at(1, 1) *= -1;
	this->uboUp.model = this->meshUp.getModel();
}


void	Chunk::draw(Engine &engine, Camera &camera, Shader &shader)
{
	this->uboUp.view = camera.getView();

	// Draw mesh up
	shader.updateUBO(engine.window, &this->uboUp);
	engine.window.drawMesh(this->meshUp, shader);
}


void	Chunk::destroy(void)
{
	this->meshUp.destroy();
	this->meshDown.destroy();
	this->meshRight.destroy();
	this->meshLeft.destroy();
	this->meshFront.destroy();
	this->meshBack.destroy();
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Chunk::generateMeshes(void)
{
	this->positions.clear();
	this->positions.resize(gm::pow(CHUNK_SIZE + 1, 3));

	int	sizeY = CHUNK_SIZE + 1;
	int	sizeZ = sizeY * sizeY;
	int	id, idYZ, idZ;
	for (int z = 0; z <= CHUNK_SIZE; z++)
	{
		idZ = z * sizeZ;
		for (int y = 0; y <= CHUNK_SIZE; y++)
		{
			idYZ = y * sizeY + idZ;
			for (int x = 0; x <= CHUNK_SIZE; x++)
			{
				id = x + idYZ;
				this->positions[id] = gm::Vec3f(x, y, z);
			}
		}
	}
}


void	Chunk::updateMeshes(void)
{
	this->createMeshUp();
}


void	Chunk::createMeshUp(void)
{
	std::vector<Vertex>		vertices;
	std::vector<uint32_t>	indices;
	int						nbVertex = 0;

	int	id;
	// Front faces
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				id = x + y * CHUNK_SIZE + z * CHUNK_SIZE2;
				if (this->cubes[id] == CUBE_AIR)
					continue;

				vertices.push_back(Vertex(gm::Vec3f(x    , y, z    ), gm::Vec3f(0, 1, 0), gm::Vec2f(0, 0)));
				vertices.push_back(Vertex(gm::Vec3f(x    , y, z + 1), gm::Vec3f(0, 1, 0), gm::Vec2f(0, 1)));
				vertices.push_back(Vertex(gm::Vec3f(x + 1, y, z + 1), gm::Vec3f(0, 1, 0), gm::Vec2f(1, 1)));
				vertices.push_back(Vertex(gm::Vec3f(x + 1, y, z    ), gm::Vec3f(0, 1, 0), gm::Vec2f(1, 0)));

				indices.push_back(nbVertex);
				indices.push_back(nbVertex + 1);
				indices.push_back(nbVertex + 2);

				indices.push_back(nbVertex);
				indices.push_back(nbVertex + 2);
				indices.push_back(nbVertex + 3);

				nbVertex += 4;
			}
		}
	}

	this->meshUp = ChunkMesh(vertices, indices);
	this->meshUp.createBuffers(*this->copyCommandPool);
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
