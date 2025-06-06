#include <program/map/Chunk.hpp>

#include <unordered_map>

//**** STATIC FUNCTIONS DEFINE *************************************************

static uint32_t	getVetrexId(
					std::unordered_map<std::size_t, uint32_t> &vertexIndex,
					std::vector<VertexPos> &vertices,
					VertexPos &vertex,
					int &nbVertex);
static char	cubeToChar(Cube cube); // TODO Remove
static char	printChunkSlide(Cube *cubes, int z); // TODO Remove

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
				{
					if (x % 2 == 0 && z % 2 == 0)
						this->cubes[id] = CUBE_GRASS;
					else if (x % 2 == 0 && z % 2 == 1)
						this->cubes[id] = CUBE_DIRT;
					else if (x % 2 == 1 && z % 2 == 0)
						this->cubes[id] = CUBE_STONE;
					else
						this->cubes[id] = CUBE_WATER;
				}
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

	this->uboPos.proj = camera.getProjection();
	this->uboPos.proj.at(1, 1) *= -1;
	this->uboPos.model = this->meshUp.getModel();
	for (int i = 0; i < CHUNK_SIZE3; i++)
		this->uboCubes.cubes[i] = this->cubes[i];
}


void	Chunk::draw(Engine &engine, Camera &camera, Shader *chunkShaders)
{
	this->uboPos.view = camera.getView();

	// Draw mesh up
	chunkShaders[SHADER_UP].updateUBO(engine.window, &this->uboPos, 0);
	chunkShaders[SHADER_UP].updateUBO(engine.window, &this->uboCubes, 1);
	engine.window.drawMesh(this->meshUp, chunkShaders[SHADER_UP]);
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
	static uint32_t	idUL;
	static uint32_t	idDR;
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

				idUL = getVetrexId(vertexIndex, vertices, pointUL, nbVertex);
				idDR = getVetrexId(vertexIndex, vertices, pointDR, nbVertex);

				// Triangle 1
				indices.push_back(idUL);
				indices.push_back(getVetrexId(vertexIndex, vertices, pointDL, nbVertex));
				indices.push_back(idDR);

				// Triangle 2
				indices.push_back(idUL);
				indices.push_back(idDR);
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


static char	cubeToChar(Cube cube)
{
	if (cube == CUBE_AIR)
		return (' ');
	else if (cube == CUBE_GRASS)
		return ('G');
	else if (cube == CUBE_DIRT)
		return ('D');
	else if (cube == CUBE_STONE)
		return ('S');
	else if (cube == CUBE_WATER)
		return ('W');
	else if (cube == CUBE_SNOW)
		return ('s');
	else if (cube == CUBE_ICE)
		return ('I');
	else if (cube == CUBE_SAND)
		return ('S');
	else if (cube == CUBE_LAVA)
		return ('L');
	else if (cube == CUBE_IRON)
		return ('i');
	else if (cube == CUBE_DIAMOND)
		return ('d');
	else
		return ('?');
}


static char	printChunkSlide(Cube *cubes, int z)
{
	for (int y = CHUNK_SIZE - 1; y >= 0; y--)
	{
		printf("|");
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			int	id = x + y * CHUNK_SIZE + z * CHUNK_SIZE2;
			if (x > 0)
				printf(" ");
			printf("%c", cubeToChar(cubes[id]));
		}
		printf("|\n");
	}
}
