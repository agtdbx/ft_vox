#include <program/map/Chunk.hpp>

#include <unordered_map>

//**** STATIC FUNCTIONS DEFINE *************************************************

static uint32_t	getVetrexId(
					std::unordered_map<std::size_t, uint32_t> &vertexIndex,
					std::vector<VertexPos> &vertices,
					VertexPos &vertex,
					int &nbVertex);

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
	this->createMeshes();

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

void	Chunk::createMeshes(void)
{
	this->createMeshUp();
}


void	Chunk::createMeshUp(void)
{
	std::vector<VertexPos>	vertices;
	std::vector<uint32_t>	indices;
	int						nbVertex = 0;
	std::unordered_map<std::size_t, uint32_t>	vertexIndex;

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

				// Point
				VertexPos pointUL(gm::Vec3f(x    , y, z    ));
				VertexPos pointDL(gm::Vec3f(x    , y, z + 1));
				VertexPos pointDR(gm::Vec3f(x + 1, y, z + 1));
				VertexPos pointUR(gm::Vec3f(x + 1, y, z    ));

				// Triangle 1
				indices.push_back(getVetrexId(vertexIndex, vertices, pointUL, nbVertex));
				indices.push_back(getVetrexId(vertexIndex, vertices, pointDL, nbVertex));
				indices.push_back(getVetrexId(vertexIndex, vertices, pointDR, nbVertex));

				// Triangle 2
				indices.push_back(getVetrexId(vertexIndex, vertices, pointUL, nbVertex));
				indices.push_back(getVetrexId(vertexIndex, vertices, pointDR, nbVertex));
				indices.push_back(getVetrexId(vertexIndex, vertices, pointUR, nbVertex));
			}
		}
	}

	this->meshUp = ChunkMesh(vertices, indices);
	this->meshUp.createBuffers(*this->copyCommandPool);
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static uint32_t	getVetrexId(
					std::unordered_map<std::size_t, uint32_t> &vertexIndex,
					std::vector<VertexPos> &vertices,
					VertexPos &vertex,
					int &nbVertex)
{
	std::size_t	hash = vertex.getHash();
	std::unordered_map<std::size_t, uint32_t>::const_iterator	it = vertexIndex.find(hash);

	// If vertex is already in vertices, return it's id
	if (it != vertexIndex.end())
		return (it->second);

	// Else add it to vertices.
	vertexIndex[hash] = nbVertex;
	vertices.push_back(vertex);

	return (nbVertex++);
}
