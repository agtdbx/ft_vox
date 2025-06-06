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

	// Draw mesh front
	// chunkShaders[SHADER_FRONT].updateUBO(engine.window, &this->uboPos, 0);
	// chunkShaders[SHADER_FRONT].updateUBO(engine.window, &this->uboCubes, 1);
	// engine.window.drawMesh(this->meshFront, chunkShaders[SHADER_FRONT]);
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
	std::vector<VertexPos>	vertices;
	std::vector<uint32_t>	indicesFront;
	std::vector<uint32_t>	indicesBack;
	std::vector<uint32_t>	indicesUp;
	std::vector<uint32_t>	indicesDown;
	std::vector<uint32_t>	indicesLeft;
	std::vector<uint32_t>	indicesRight;
	int						nbVertex = 0;
	std::unordered_map<std::size_t, uint32_t>	vertexIndex;

	int	id;
	static uint32_t RUF_id;
	static uint32_t RDF_id;
	static uint32_t LUF_id;
	static uint32_t LDF_id;
	static uint32_t RUB_id;
	static uint32_t RDB_id;
	static uint32_t LUB_id;
	static uint32_t LDB_id;

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
				VertexPos pointRUF(gm::Vec3f(x + 1, y    , z + 1));
				VertexPos pointRDF(gm::Vec3f(x + 1, y - 1, z + 1));
				VertexPos pointLUF(gm::Vec3f(x    , y    , z + 1));
				VertexPos pointLDF(gm::Vec3f(x    , y - 1, z + 1));
				VertexPos pointRUB(gm::Vec3f(x + 1, y    , z    ));
				VertexPos pointRDB(gm::Vec3f(x + 1, y - 1, z    ));
				VertexPos pointLUB(gm::Vec3f(x    , y    , z    ));
				VertexPos pointLDB(gm::Vec3f(x    , y - 1, z    ));

				//Id
				RUF_id = getVetrexId(vertexIndex, vertices, pointRUF, nbVertex);
				RDF_id = getVetrexId(vertexIndex, vertices, pointRDF, nbVertex);
				LUF_id = getVetrexId(vertexIndex, vertices, pointLUF, nbVertex);
				LDF_id = getVetrexId(vertexIndex, vertices, pointLDF, nbVertex);
				RUB_id = getVetrexId(vertexIndex, vertices, pointRUB, nbVertex);
				RDB_id = getVetrexId(vertexIndex, vertices, pointRDB, nbVertex);
				LUB_id = getVetrexId(vertexIndex, vertices, pointLUB, nbVertex);
				LDB_id = getVetrexId(vertexIndex, vertices, pointLDB, nbVertex);

				// Face Up
				// Triangle 1
				indicesUp.push_back(LUB_id);
				indicesUp.push_back(LUF_id);
				indicesUp.push_back(RUF_id);

				// Triangle 2
				indicesUp.push_back(LUB_id);
				indicesUp.push_back(RUF_id);
				indicesUp.push_back(RUB_id);

				// Face Front
				// Triangle 1
				indicesFront.push_back(LUF_id);
				indicesFront.push_back(LDF_id);
				indicesFront.push_back(RDF_id);

				// Triangle 2
				indicesFront.push_back(LUF_id);
				indicesFront.push_back(RDF_id);
				indicesFront.push_back(RUF_id);

				// Face Right
				// Triangle 1
				indicesRight.push_back(RUF_id);
				indicesRight.push_back(RDF_id);
				indicesRight.push_back(RDB_id);

				// Triangle 2
				indicesRight.push_back(RUF_id);
				indicesRight.push_back(RDB_id);
				indicesRight.push_back(RUB_id);

				// Face Left
				// Triangle 1
				indicesLeft.push_back(LUB_id);
				indicesLeft.push_back(LDB_id);
				indicesLeft.push_back(LDF_id);

				// Triangle 2
				indicesLeft.push_back(LUB_id);
				indicesLeft.push_back(LDF_id);
				indicesLeft.push_back(LUF_id);

				// Face Down
				// Triangle 1
				indicesDown.push_back(LDF_id);
				indicesDown.push_back(LDB_id);
				indicesDown.push_back(RDB_id);

				// Triangle 2
				indicesDown.push_back(LDF_id);
				indicesDown.push_back(RDB_id);
				indicesDown.push_back(RDF_id);

				// Face Back
				// Triangle 1
				indicesBack.push_back(LUB_id);
				indicesBack.push_back(LDB_id);
				indicesBack.push_back(RDB_id);

				// Triangle 2
				indicesBack.push_back(LUB_id);
				indicesBack.push_back(RDB_id);
				indicesBack.push_back(RUF_id);
			}
		}
	}

	this->meshFront = ChunkMesh(vertices, indicesFront);
	this->meshFront.createBuffers(*this->copyCommandPool);

	this->meshUp = ChunkMesh(vertices, indicesUp);
	this->meshUp.createBuffers(*this->copyCommandPool);

	this->meshBack = ChunkMesh(vertices, indicesBack);
	this->meshBack.createBuffers(*this->copyCommandPool);

	this->meshLeft = ChunkMesh(vertices, indicesLeft);
	this->meshLeft.createBuffers(*this->copyCommandPool);

	this->meshRight = ChunkMesh(vertices, indicesRight);
	this->meshRight.createBuffers(*this->copyCommandPool);

	this->meshDown = ChunkMesh(vertices, indicesDown);
	this->meshDown.createBuffers(*this->copyCommandPool);
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
