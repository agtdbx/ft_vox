#include <program/map/Chunk.hpp>

#include <unordered_map>

//**** STATIC FUNCTIONS DEFINE *************************************************

static uint32_t	getVetrexId(
					std::unordered_map<std::size_t, uint32_t> &vertexIndex,
					std::vector<VertexPos> &vertices,
					VertexPos &vertex,
					int &nbVertex);
static char	cubeToChar(Cube cube); // TODO Remove
static void	printChunkSlide(Cube *cubes, int z); // TODO Remove

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

	this->setCube(8, 9, 8, CUBE_LAVA);
	this->setCube(6, 12, 6, CUBE_DIAMOND);
	this->setCube(20, 16, 12, CUBE_GRASS);
}


Chunk::Chunk(const Chunk &obj)
{
	for (int i = 0; i < CHUNK_SIZE3; i++)
		this->cubes[i] = obj.cubes[i];
	this->copyCommandPool = obj.copyCommandPool;
	this->mesh = obj.mesh;
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


Cube	Chunk::getCube(const gm::Vec3u &pos)
{
	if (pos.x >= CHUNK_SIZE || pos.y >= CHUNK_SIZE || pos.z >= CHUNK_SIZE)
		return (CUBE_AIR);
	return (this->cubes[pos.x + pos.y * CHUNK_SIZE + pos.z * CHUNK_SIZE2]);
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

	this->mesh = obj.mesh;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Chunk::init(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader,
				const gm::Vec3f &position)
{
	this->copyCommandPool = &engine.commandPool;
	this->createMeshes();

	this->mesh.setPosition(position);

	this->uboPos.proj = camera.getProjection();
	this->uboPos.proj.at(1, 1) *= -1;
	this->uboPos.model = this->mesh.getModel();
	this->uboPos.pos = gm::Vec4f(this->mesh.getPosition());

	for (int i = 0; i < CHUNK_SIZE3; i++)
		this->uboCubes.cubes[i] = this->cubes[i];

	this->descriptorSets = chunkShader.shader.getDescriptorSets();
	// this->descriptorSets = chunkShader.shader.createNewDescriptorSets(engine, CUBE_TEXTURES);
	this->descriptorSetsFdf = chunkShader.shaderFdf.getDescriptorSets();
	// this->descriptorSetsFdf = chunkShader.shaderFdf.createNewDescriptorSets(engine, CUBE_TEXTURES);
}


void	Chunk::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	this->uboPos.view = camera.getView();

	// Draw mesh
	if (!chunkShader.shaderFdfEnable)
	{
		chunkShader.shader.updateUBO(engine.window, &this->uboPos, 0);
		chunkShader.shader.updateUBO(engine.window, &this->uboCubes, 1);
		engine.window.drawMesh(this->mesh, chunkShader.shader, this->descriptorSets);
	}
	else
	{
		chunkShader.shaderFdf.updateUBO(engine.window, &this->uboPos, 0);
		chunkShader.shaderFdf.updateUBO(engine.window, &this->uboCubes, 1);
		engine.window.drawMesh(this->mesh, chunkShader.shaderFdf, this->descriptorSetsFdf);
	}
}


void	Chunk::destroy(void)
{
	this->mesh.destroy();
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Chunk::createMeshes(void)
{
	std::unordered_map<std::size_t, uint32_t>	vertexIndex;
	std::vector<VertexPos>	vertices;
	std::vector<uint32_t>	indices;
	int						nbVertex = 0;
	const gm::Vec3f			normalUp(0, 1, 0);
	const gm::Vec3f			normalDown(0, -1, 0);
	const gm::Vec3f			normalFront(0, 0, 1);
	const gm::Vec3f			normalBack(0, 0, -1);
	const gm::Vec3f			normalLeft(-1, 0, 0);
	const gm::Vec3f			normalRight(1, 0, 0);

	int			id;
	gm::Vec3f	pointLUF, pointLDF, pointRUF, pointRDF,
				pointLUB, pointLDB, pointRUB, pointRDB;

	for (unsigned int x = 0; x < CHUNK_SIZE; x++)
	{
		for (unsigned int y = 0; y < CHUNK_SIZE; y++)
		{
			for (unsigned int z = 0; z < CHUNK_SIZE; z++)
			{
				id = x + y * CHUNK_SIZE + z * CHUNK_SIZE2;
				if (this->cubes[id] == CUBE_AIR)
					continue;

				// Points
				pointLUF = gm::Vec3f(x    , y + 1, z + 1);
				pointLDF = gm::Vec3f(x    , y    , z + 1);
				pointRUF = gm::Vec3f(x + 1, y + 1, z + 1);
				pointRDF = gm::Vec3f(x + 1, y    , z + 1);
				pointLUB = gm::Vec3f(x    , y + 1, z    );
				pointLDB = gm::Vec3f(x    , y    , z    );
				pointRUB = gm::Vec3f(x + 1, y + 1, z    );
				pointRDB = gm::Vec3f(x + 1, y    , z    );

				// Face up
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x, y + 1, z},
									pointLUB, pointLUF, pointRUF, pointRUB, normalUp);
				// Face down
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x, y - 1, z},
									pointLDF, pointLDB, pointRDB, pointRDF, normalDown);
				// Face front
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x, y, z + 1},
									pointLUF, pointLDF, pointRDF, pointRUF, normalFront);
				// Face back
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x, y, z - 1},
									pointRUB, pointRDB, pointLDB, pointLUB, normalBack);
				// Face right
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x + 1, y, z},
									pointRUF, pointRDF, pointRDB, pointRUB, normalRight);
				// Face left
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x - 1, y, z},
									pointLUB, pointLDB, pointLDF, pointLUF, normalLeft);
			}
		}
	}

	this->mesh = ChunkMesh(vertices, indices);
	this->mesh.createBuffers(*this->copyCommandPool);
}


void	Chunk::createFace(
				std::unordered_map<std::size_t, uint32_t> &vertexIndex,
				std::vector<VertexPos> &vertices,
				std::vector<uint32_t> &indices,
				int &nbVertex,
				const gm::Vec3u &posCheck,
				const gm::Vec3f &posLU,
				const gm::Vec3f &posLD,
				const gm::Vec3f &posRD,
				const gm::Vec3f &posRU,
				const gm::Vec3f &normal)
{
	if (this->getCube(posCheck) != CUBE_AIR)
		return ;

	VertexPos pointLU(posLU, normal);
	VertexPos pointLD(posLD, normal);
	VertexPos pointRD(posRD, normal);
	VertexPos pointRU(posRU, normal);

	uint32_t	LU_id = getVetrexId(vertexIndex, vertices, pointLU, nbVertex);
	uint32_t	LD_id = getVetrexId(vertexIndex, vertices, pointLD, nbVertex);
	uint32_t	RD_id = getVetrexId(vertexIndex, vertices, pointRD, nbVertex);
	uint32_t	RU_id = getVetrexId(vertexIndex, vertices, pointRU, nbVertex);

	indices.push_back(LU_id);
	indices.push_back(LD_id);
	indices.push_back(RD_id);

	indices.push_back(LU_id);
	indices.push_back(RD_id);
	indices.push_back(RU_id);
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


static void	printChunkSlide(Cube *cubes, int z)
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
