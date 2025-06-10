#include <program/map/Chunk.hpp>

#include <program/map/Map.hpp>

#include <unordered_map>

//**** STATIC FUNCTIONS DEFINE *************************************************

static uint32_t	getVetrexId(
					std::unordered_map<std::size_t, uint32_t> &vertexIndex,
					std::vector<VertexPosNrm> &vertices,
					VertexPosNrm &vertex,
					int &nbVertex);

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Chunk::Chunk(void)
{
	this->chunkId = gm::Vec2i(0);
	this->chunkPosition = gm::Vec3f(0);
	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
		this->cubes[i] = CUBE_AIR;
	this->copyCommandPool = NULL;
}


Chunk::Chunk(const Chunk &obj)
{
	this->chunkId = obj.chunkId;
	this->chunkPosition = obj.chunkPosition;
	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
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

Cube	Chunk::getCube(int x, int y, int z)
{
	if (x < 0 || x >= CHUNK_SIZE
		|| y < 0 || y >= CHUNK_HEIGHT
		|| z < 0 || z >= CHUNK_SIZE)
		return (CUBE_AIR);
	return (this->cubes[x + z * CHUNK_SIZE + y * CHUNK_SIZE2]);
}


Cube	Chunk::getCube(const gm::Vec3i &pos)
{
	if (pos.x < 0 || pos.x >= CHUNK_SIZE
		|| pos.y < 0 || pos.y >= CHUNK_HEIGHT
		|| pos.z < 0 || pos.z >= CHUNK_SIZE)
		return (CUBE_AIR);
	return (this->cubes[pos.x + pos.z * CHUNK_SIZE + pos.y * CHUNK_SIZE2]);
}

//---- Setters -----------------------------------------------------------------

void	Chunk::setCube(int x, int y, int z, Cube cube)
{
	if (x < 0 || x >= CHUNK_SIZE
		|| y < 0 || y >= CHUNK_HEIGHT
		|| z < 0 || z >= CHUNK_SIZE)
		return ;
	this->cubes[x + z * CHUNK_SIZE + y * CHUNK_SIZE2] = cube;
}

//---- Operators ---------------------------------------------------------------

Chunk	&Chunk::operator=(const Chunk &obj)
{
	if (this == &obj)
		return (*this);

	this->chunkId = obj.chunkId;
	this->chunkPosition = obj.chunkPosition;

	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
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
				ChunkShader &chunkShader)
{
	this->copyCommandPool = &engine.commandPool;

	chunkShader.shader.initShaderParam(engine, this->shaderParam, CUBE_TEXTURES);
	chunkShader.shaderFdf.initShaderParam(engine, this->shaderParamFdf, {});
	chunkShader.shaderBorder.initShaderParam(engine, this->shaderParamBorder, {});

	this->uboPos.proj = camera.getProjection();
	this->uboPos.proj.at(1, 1) *= -1;

	this->createBorderMesh();
}


void	Chunk::generate(const gm::Vec2i &chunkId)
{
	this->chunkId = chunkId;
	this->chunkPosition.x = this->chunkId.x * CHUNK_SIZE;
	this->chunkPosition.y = 0.0f;
	this->chunkPosition.z = this->chunkId.y * CHUNK_SIZE;

	int	idY, idZ;
	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		idY = y * CHUNK_SIZE2;
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			idZ = z * CHUNK_SIZE;
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				int	id = x + idZ + idY;
				if (y > 80)
					this->cubes[id] = CUBE_AIR;
				else if (y == 80)
					this->cubes[id] = CUBE_GRASS;
				else if (y >= 77)
					this->cubes[id] = CUBE_DIRT;
				else
					this->cubes[id] = CUBE_STONE;
			}
		}
	}

	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
		this->uboCubes.cubes[i] = this->cubes[i];
}


void	Chunk::updateMeshes(Map &map)
{
	this->createMesh(map);
	this->mesh.setPosition(this->chunkPosition);
	this->uboPos.model = this->mesh.getModel();
	this->uboPos.pos = gm::Vec4f(this->mesh.getPosition());
}


void	Chunk::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	this->uboPos.view = camera.getView();

	// Draw mesh
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
	int	nbVertex = 0;
	std::vector<VertexPos>	vertices;
	std::vector<uint32_t>	indices;
	float	h0, h1;

	for (int i = 0; i < 8; i++)
	{
		h0 = CHUNK_SIZE * i;
		h1 = CHUNK_SIZE * (i + 1);

		vertices.push_back(VertexPos({0,          h1, CHUNK_SIZE})); // LUF 0
		vertices.push_back(VertexPos({0,          h0, CHUNK_SIZE})); // LDF 1
		vertices.push_back(VertexPos({CHUNK_SIZE, h1, CHUNK_SIZE})); // RUF 2
		vertices.push_back(VertexPos({CHUNK_SIZE, h0, CHUNK_SIZE})); // RDF 3
		vertices.push_back(VertexPos({0,          h1, 0         })); // LUB 4
		vertices.push_back(VertexPos({0,          h0, 0         })); // LDB 5
		vertices.push_back(VertexPos({CHUNK_SIZE, h1, 0         })); // RUB 6
		vertices.push_back(VertexPos({CHUNK_SIZE, h0, 0         })); // RDB 7

		// Front face
		indices.push_back(nbVertex + 0);indices.push_back(nbVertex + 1);indices.push_back(nbVertex + 2);
		indices.push_back(nbVertex + 0);indices.push_back(nbVertex + 2);indices.push_back(nbVertex + 3);
		// Back face
		indices.push_back(nbVertex + 6);indices.push_back(nbVertex + 7);indices.push_back(nbVertex + 5);
		indices.push_back(nbVertex + 6);indices.push_back(nbVertex + 5);indices.push_back(nbVertex + 4);
		// Left face
		indices.push_back(nbVertex + 4);indices.push_back(nbVertex + 5);indices.push_back(nbVertex + 1);
		indices.push_back(nbVertex + 4);indices.push_back(nbVertex + 1);indices.push_back(nbVertex + 0);
		// Right face
		indices.push_back(nbVertex + 2);indices.push_back(nbVertex + 3);indices.push_back(nbVertex + 7);
		indices.push_back(nbVertex + 2);indices.push_back(nbVertex + 7);indices.push_back(nbVertex + 6);

		nbVertex += 8;
	}

	this->borderMesh = ChunkBorderMesh(vertices, indices);
	this->borderMesh.createBuffers(*this->copyCommandPool);
}


void	Chunk::createMesh(Map &map)
{
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
	Chunk	*leftChunk = map.getChunk(this->chunkId.x - 1, this->chunkId.y);
	Chunk	*rightChunk = map.getChunk(this->chunkId.x + 1, this->chunkId.y);
	Chunk	*frontChunk = map.getChunk(this->chunkId.x, this->chunkId.y + 1);
	Chunk	*backChunk = map.getChunk(this->chunkId.x, this->chunkId.y - 1);

	int			id, idY, idZ;
	gm::Vec3f	pointLUF, pointLDF, pointRUF, pointRDF,
				pointLUB, pointLDB, pointRUB, pointRDB;

	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		idY = y * CHUNK_SIZE2;
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			idZ = z * CHUNK_SIZE;
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				id = x + idZ + idY;
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
									leftChunk, rightChunk, frontChunk, backChunk,
									pointLUB, pointLUF, pointRUF, pointRUB, normalUp);
				// Face down
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x, y - 1, z},
									leftChunk, rightChunk, frontChunk, backChunk,
									pointLDF, pointLDB, pointRDB, pointRDF, normalDown);
				// Face front
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x, y, z + 1},
									leftChunk, rightChunk, frontChunk, backChunk,
									pointLUF, pointLDF, pointRDF, pointRUF, normalFront);
				// Face back
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x, y, z - 1},
									leftChunk, rightChunk, frontChunk, backChunk,
									pointRUB, pointRDB, pointLDB, pointLUB, normalBack);
				// Face right
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x + 1, y, z},
									leftChunk, rightChunk, frontChunk, backChunk,
									pointRUF, pointRDF, pointRDB, pointRUB, normalRight);
				// Face left
				this->createFace(vertexIndex, vertices, indices, nbVertex, {x - 1, y, z},
									leftChunk, rightChunk, frontChunk, backChunk,
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
				gm::Vec3i posCheck,
				Chunk *leftChunk,
				Chunk *rightChunk,
				Chunk *frontChunk,
				Chunk *backChunk,
				const gm::Vec3f &posLU,
				const gm::Vec3f &posLD,
				const gm::Vec3f &posRD,
				const gm::Vec3f &posRU,
				const gm::Vec3f &normal)
{
	if (posCheck.x < 0)
	{
		if (leftChunk)
		{
			posCheck.x += CHUNK_SIZE;
			if (leftChunk->getCube(posCheck) != CUBE_AIR)
				return ;
		}
	}
	else if (posCheck.x >= CHUNK_SIZE)
	{
		if (rightChunk)
		{
			posCheck.x -= CHUNK_SIZE;
			if (rightChunk->getCube(posCheck) != CUBE_AIR)
				return ;
		}
	}
	else if (posCheck.z < 0)
	{
		if (backChunk)
		{
			posCheck.z += CHUNK_SIZE;
			if (backChunk->getCube(posCheck) != CUBE_AIR)
				return ;
		}
	}
	else if (posCheck.z >= CHUNK_SIZE)
	{
		if (frontChunk)
		{
			posCheck.z -= CHUNK_SIZE;
			if (frontChunk->getCube(posCheck) != CUBE_AIR)
				return ;
		}
	}
	else if (this->getCube(posCheck) != CUBE_AIR)
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
