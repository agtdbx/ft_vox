#include <program/map/Chunk.hpp>

#include <unordered_map>

//**** STATIC FUNCTIONS DEFINE *************************************************

static uint32_t	getVetrexId(
					std::unordered_map<std::size_t, uint32_t> &vertexIndex,
					std::vector<VertexPosNrm> &vertices,
					VertexPosNrm &vertex,
					int &nbVertex);
static char	cubeToChar(Cube cube); // TODO Remove
static void	printChunkSlide(Cube *cubes, int z); // TODO Remove

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Chunk::Chunk(void)
{
	this->chunkId = gm::Vec3i(0);
	this->chunkPosition = gm::Vec3f(0);
	for (int i = 0; i < CHUNK_SIZE3; i++)
		this->cubes[i] = CUBE_AIR;
	this->copyCommandPool = NULL;
	this->empty = true;
}


Chunk::Chunk(const Chunk &obj)
{
	this->chunkId = obj.chunkId;
	this->chunkPosition = obj.chunkPosition;
	for (int i = 0; i < CHUNK_SIZE3; i++)
		this->cubes[i] = obj.cubes[i];
	this->copyCommandPool = obj.copyCommandPool;
	this->mesh = obj.mesh;
	this->empty = obj.empty;
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

	this->chunkId = obj.chunkId;
	this->chunkPosition = obj.chunkPosition;

	for (int i = 0; i < CHUNK_SIZE3; i++)
		this->cubes[i] = obj.cubes[i];

	if (!this->copyCommandPool)
		this->copyCommandPool = obj.copyCommandPool;

	this->mesh = obj.mesh;
	this->empty = obj.empty;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Chunk::init(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader,
				const gm::Vec3i &chunkId)
{
	this->chunkId = chunkId;
	this->chunkPosition.x = this->chunkId.x * CHUNK_SIZE;
	this->chunkPosition.y = this->chunkId.y * CHUNK_SIZE;
	this->chunkPosition.z = this->chunkId.z * CHUNK_SIZE;

	this->initBlocks();

	this->copyCommandPool = &engine.commandPool;
	this->createBorderMesh();
	this->createMesh();

	this->mesh.setPosition(this->chunkPosition);

	this->uboPos.proj = camera.getProjection();
	this->uboPos.proj.at(1, 1) *= -1;
	this->uboPos.model = this->mesh.getModel();
	this->uboPos.pos = gm::Vec4f(this->mesh.getPosition());

	for (int i = 0; i < CHUNK_SIZE3; i++)
		this->uboCubes.cubes[i] = this->cubes[i];

	chunkShader.shader.initShaderParam(engine, this->shaderParam, CUBE_TEXTURES);
	chunkShader.shaderFdf.initShaderParam(engine, this->shaderParamFdf, {});
	chunkShader.shaderBorder.initShaderParam(engine, this->shaderParamBorder, {});
}


void	Chunk::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	this->uboPos.view = camera.getView();

	// Draw mesh
	if (!this->empty)
	{
		if (!chunkShader.shaderFdfEnable)
		{
			this->shaderParam.updateUBO(engine.window, &this->uboPos, 0);
			this->shaderParam.updateUBO(engine.window, &this->uboCubes, 1);
			engine.window.drawMesh(this->mesh, chunkShader.shader, this->shaderParam);
		}
		else
		{
			this->shaderParamFdf.updateUBO(engine.window, &this->uboPos, 0);
			engine.window.drawMesh(this->mesh, chunkShader.shaderFdf, this->shaderParamFdf);
		}
	}

	if (chunkShader.shaderBorderEnable)
	{
		this->shaderParamBorder.updateUBO(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->borderMesh, chunkShader.shaderBorder, this->shaderParamBorder);
	}
}


void	Chunk::destroy(Engine &engine)
{
	this->shaderParam.destroy(engine);
	this->shaderParamFdf.destroy(engine);
	this->mesh.destroy();
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Chunk::createBorderMesh(void)
{
	std::vector<VertexPos>	vertices = {
		{{0,          CHUNK_SIZE, CHUNK_SIZE}}, // LUF 0
		{{0,          0,          CHUNK_SIZE}}, // LDF 1
		{{CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE}}, // RUF 2
		{{CHUNK_SIZE, 0,          CHUNK_SIZE}}, // RDF 3
		{{0,          CHUNK_SIZE, 0         }}, // LUB 4
		{{0,          0,          0         }}, // LDB 5
		{{CHUNK_SIZE, CHUNK_SIZE, 0         }}, // RUB 6
		{{CHUNK_SIZE, 0,          0         }}, // RDB 7
	};
	std::vector<uint32_t>	indices = {
		// Front face
		0, 1, 2,
		0, 2, 3,
		// Back face
		6, 7, 5,
		6, 5, 4,
		// Left face
		4, 5, 1,
		4, 1, 0,
		// Right face
		2, 3, 7,
		2, 7, 6,
		// Up face
		4, 0, 2,
		4, 2, 6,
		// Down face
		1, 4, 7,
		1, 7, 3,
	};

	this->borderMesh = ChunkBorderMesh(vertices, indices);
	this->borderMesh.createBuffers(*this->copyCommandPool);
}

void	Chunk::createMesh(void)
{
	if (this->empty)
		return ;

	std::unordered_map<std::size_t, uint32_t>	vertexIndex;
	std::vector<VertexPosNrm>	vertices;
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
				std::vector<VertexPosNrm> &vertices,
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

	VertexPosNrm pointLU(posLU, normal);
	VertexPosNrm pointLD(posLD, normal);
	VertexPosNrm pointRD(posRD, normal);
	VertexPosNrm pointRU(posRU, normal);

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

//TODO laissez se générer des chunk sous le chunk

void	Chunk::initBlocks(void)
{
	float maxSize = 0;
	float perlinX = 0;
	float perlinZ = 0;
	float tmpX = 0;
	float tmpZ = 0;
	if (this->chunkId.y > 0)
		return ;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			tmpX = ((float)this->chunkId.x);
			if (tmpX < 0)
			{
				tmpX = tmpX * -1;
				perlinX = tmpX - ((float)x / 32);
			}
			else
				perlinX = tmpX + ((float)x / 32);
			tmpZ = ((float)this->chunkId.z);
			if (tmpZ < 0)
			{
				tmpZ = tmpZ * -1;
				perlinZ = tmpZ - ((float)z / 32);
			}
			else
				perlinZ = tmpZ + ((float)z / 32);
			maxSize = perlin(perlinX, perlinZ);
			//TODO seed here maybe ?
			perlinX = perlinX + (SEED & 0xff);
			perlinZ = perlinZ + ((SEED >> 16) & 0xff);
			maxSize = maxSize + (perlin(perlinX, perlinZ) / 2.5);
			for (int y = 0; y < CHUNK_SIZE; y++)
			{
				int	id = x + y * CHUNK_SIZE + z * CHUNK_SIZE2;


				//Basic plaine
				//TODO change blocType with perlin noise for different biome
				if (y > (int)maxSize && y > 8)
					this->cubes[id] = CUBE_AIR;
				else if (y > (int)maxSize && y <= 8)
				{
					this->empty = false;
					this->cubes[id] = CUBE_WATER;
				}
				else
				{
					this->empty = false;
					if (y > 4 && y == (int)maxSize)
						this->cubes[id] = CUBE_GRASS;
					else if (y > 4 && y < (int)maxSize)
						this->cubes[id] = CUBE_DIRT;
					else if (y <= 4 && y != 0)
						this->cubes[id] = CUBE_STONE;
					else
						this->cubes[id] = CUBE_DIAMOND;
				}
			}
		}
	}
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static uint32_t	getVetrexId(
					std::unordered_map<std::size_t, uint32_t> &vertexIndex,
					std::vector<VertexPosNrm> &vertices,
					VertexPosNrm &vertex,
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
