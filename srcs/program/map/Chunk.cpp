#include <program/map/Chunk.hpp>

#include <program/map/Map.hpp>

#include <unordered_map>

//**** STATIC FUNCTIONS DEFINE *************************************************

const double	INV_CLOCKS_PER_USEC = 1.0 / (double)CLOCKS_PER_SEC * 1000000.0;
const gm::Vec3f	normalUp(0, 1, 0);
const gm::Vec3f	normalDown(0, -1, 0);
const gm::Vec3f	normalFront(0, 0, 1);
const gm::Vec3f	normalBack(0, 0, -1);
const gm::Vec3f	normalLeft(-1, 0, 0);
const gm::Vec3f	normalRight(1, 0, 0);

static uint32_t	getVetrexId(
					std::unordered_map<std::size_t, uint32_t> &vertexIndex,
					std::vector<VertexPosNrm> &vertices,
					VertexPosNrm &vertex,
					int &nbVertex);
static void	createTriangleFace(
				std::unordered_map<std::size_t, uint32_t> &vertexIndex,
				std::vector<VertexPosNrm> &vertices,
				std::vector<uint32_t> &indices,
				int &nbVertex,
				const gm::Vec3f &posLU,
				const gm::Vec3f &posLD,
				const gm::Vec3f &posRD,
				const gm::Vec3f &posRU,
				const gm::Vec3f &normal);

// TODO : Remove
void	startLog(PerfField &perfField)
{
	perfField.start = std::clock();
}

void	endLog(PerfField &perfField)
{
	perfField.total += (std::clock() - perfField.start) * INV_CLOCKS_PER_USEC;
	perfField.nbCall++;
}

void	resetLog(PerfField &perfField)
{
	perfField.start = 0.0;
	perfField.total = 0.0;
	perfField.nbCall = 0;
}

void	printLog(PerfField &perfField, const char *msg)
{
	int	avg = perfField.total / perfField.nbCall;

	printf("%s : total %i us, nb call %i, avg %i us\n",
			msg, perfField.total, perfField.nbCall, avg);
}

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Chunk::Chunk(void)
{
	this->chunkId = gm::Vec2i(0);
	this->chunkPosition = gm::Vec3f(0);
	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
		this->cubes[i] = CUBE_AIR;
	for (int i = 0; i < CHUNK_MASK_SIZE; i++)
	{
		this->cubesMaskLeft[i] = 0;
		this->cubesMaskRight[i] = 0;
		this->cubesMaskFront[i] = 0;
		this->cubesMaskBack[i] = 0;
	}
	this->copyCommandPool = NULL;
}


Chunk::Chunk(const Chunk &obj)
{
	this->chunkId = obj.chunkId;
	this->chunkPosition = obj.chunkPosition;
	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
		this->cubes[i] = obj.cubes[i];
	for (int i = 0; i < CHUNK_MASK_SIZE; i++)
	{
		this->cubesMaskLeft[i] = obj.cubesMaskLeft[i];
		this->cubesMaskRight[i] = obj.cubesMaskRight[i];
		this->cubesMaskFront[i] = obj.cubesMaskFront[i];
		this->cubesMaskBack[i] = obj.cubesMaskBack[i];
	}
	this->copyCommandPool = obj.copyCommandPool;
	this->mesh = obj.mesh;
}

//---- Destructor --------------------------------------------------------------

Chunk::~Chunk()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

Cube	Chunk::getCube(int x, int y, int z) const
{
	if (x < 0 || x >= CHUNK_SIZE
		|| y < 0 || y >= CHUNK_HEIGHT
		|| z < 0 || z >= CHUNK_SIZE)
		return (CUBE_AIR);
	return (this->cubes[x + z * CHUNK_SIZE + y * CHUNK_SIZE2]);
}


Cube	Chunk::getCube(const gm::Vec3i &pos) const
{
	if (pos.x < 0 || pos.x >= CHUNK_SIZE
		|| pos.y < 0 || pos.y >= CHUNK_HEIGHT
		|| pos.z < 0 || pos.z >= CHUNK_SIZE)
		return (CUBE_AIR);
	return (this->cubes[pos.x + pos.z * CHUNK_SIZE + pos.y * CHUNK_SIZE2]);
}


const Cube	&Chunk::at(int x, int y, int z) const
{
	return (this->cubes[x + z * CHUNK_SIZE + y * CHUNK_SIZE2]);
}


const Cube	&Chunk::at(const gm::Vec3i &pos) const
{
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

	for (int i = 0; i < CHUNK_MASK_SIZE; i++)
	{
		this->cubesMaskLeft[i] = obj.cubesMaskLeft[i];
		this->cubesMaskRight[i] = obj.cubesMaskRight[i];
		this->cubesMaskFront[i] = obj.cubesMaskFront[i];
		this->cubesMaskBack[i] = obj.cubesMaskBack[i];
	}

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


void	Chunk::generate(const gm::Vec2i &chunkId, PerfLogger &perfLogger)
{
	this->chunkId = chunkId;
	this->chunkPosition.x = this->chunkId.x * CHUNK_SIZE;
	this->chunkPosition.y = 0.0f;
	this->chunkPosition.z = this->chunkId.y * CHUNK_SIZE;

	startLog(perfLogger.generation);

	float maxSize = 0;
	float perlinX = 0;
	float perlinZ = 0;
	float tmpX = 0;
	float tmpZ = 0;
	int		idZ, id;
	for (int z = 0; z < CHUNK_SIZE; z++)
	{
		idZ = z * CHUNK_SIZE;
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			tmpX = ((float)this->chunkId.x);
			if (tmpX < 0)
			{
				tmpX = tmpX * -1;
				perlinX = tmpX - ((float)x / CHUNK_SIZE);
			}
			else
				perlinX = tmpX + ((float)x / CHUNK_SIZE);
			tmpZ = ((float)this->chunkId.y);
			if (tmpZ < 0)
			{
				tmpZ = tmpZ * -1;
				perlinZ = tmpZ - ((float)z / CHUNK_SIZE);
			}
			else
				perlinZ = tmpZ + ((float)z / CHUNK_SIZE);
			maxSize = perlin(perlinX, perlinZ);
			//TODO seed here maybe ?
			perlinX = perlinX + (SEED & 0xff);
			perlinZ = perlinZ + ((SEED >> 16) & 0xff);
			maxSize = maxSize + (perlin(perlinX, perlinZ) / 2.5);
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				//Basic plaine
				//TODO change blocType with perlin noise for different biome
				if (y > (int)maxSize && y > 40)
					break;

				id = x + idZ + y * CHUNK_SIZE2;
				// maskY = y * CHUNK_SIZE;
				// this->cubesMaskFront[x + maskY] |= (0b1 << z);
				// this->cubesMaskBack[x + maskY] |= (0b1 << (CHUNK_MAX - z));
				// this->cubesMaskLeft[z + maskY] |= (0b1 << x);
				// this->cubesMaskRight[z + maskY] |= (0b1 << (CHUNK_MAX - x));

				if (y > (int)maxSize && y <= 40)
					this->cubes[id] = CUBE_WATER;
				else
				{
					if (y > 40 && y == (int)maxSize)
						this->cubes[id] = CUBE_GRASS;
					else if (y > 40 && y > (int)maxSize - 3 && y < (int)maxSize)
						this->cubes[id] = CUBE_DIRT;
					else if (y != 0)
						this->cubes[id] = CUBE_STONE;
					else
						this->cubes[id] = CUBE_DIAMOND;
				}
			}
		}
	}

	endLog(perfLogger.generation);

	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
		this->uboCubes.cubes[i] = this->cubes[i];
}


void	Chunk::updateMeshes(Map &map, PerfLogger &perfLogger)
{
	startLog(perfLogger.createMesh);
	this->createMesh(map);
	endLog(perfLogger.createMesh);

	perfLogger.nbTriangles += this->mesh.getNbIndex() / 3;

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

/*
BASIC MESHING
Chunk generation : total 31567 us, nb call 64, avg 493 us
Mesh creation : total 338274 us, nb call 64, avg 5285 us
Number of triangle : 348290

*/

void	Chunk::createMesh(Map &map)
{
	std::unordered_map<std::size_t, uint32_t>	vertexIndex;
	std::vector<Face>			facesFront;
	std::vector<Face>			facesBack;
	std::vector<Face>			facesRight;
	std::vector<Face>			facesLeft;
	std::vector<Face>			facesUp;
	std::vector<Face>			facesDown;
	std::vector<VertexPosNrm>	vertices;
	std::vector<uint32_t>		indices;
	int						nbVertex = 0;
	Chunk	*leftChunk = map.getChunk(this->chunkId.x - 1, this->chunkId.y);
	Chunk	*rightChunk = map.getChunk(this->chunkId.x + 1, this->chunkId.y);
	Chunk	*frontChunk = map.getChunk(this->chunkId.x, this->chunkId.y + 1);
	Chunk	*backChunk = map.getChunk(this->chunkId.x, this->chunkId.y - 1);

	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				if (this->at(x, y, z) == CUBE_AIR)
					continue;

				// Create face front
				if (z + 1 < CHUNK_SIZE)
				{
					if (this->at(x, y, z + 1) == CUBE_AIR)
						facesFront.push_back({x, y, 1, 1, z + 1});
				}
				else if (frontChunk)
				{
					if (frontChunk->at(x, y, 0) == CUBE_AIR)
						facesFront.push_back({x, y, 1, 1, z + 1});
				}

				// Create face back
				if (z - 1 >= 0)
				{
					if (this->at(x, y, z - 1) == CUBE_AIR)
						facesBack.push_back({x, y, 1, 1, z});
				}
				else if (backChunk)
				{
					if (backChunk->at(x, y, CHUNK_MAX) == CUBE_AIR)
						facesBack.push_back({x, y, 1, 1, z});
				}

				// Create face right
				if (x + 1 < CHUNK_SIZE)
				{
					if (this->at(x + 1, y, z) == CUBE_AIR)
						facesRight.push_back({z, y, 1, 1, x + 1});
				}
				else if (rightChunk)
				{
					if (rightChunk->at(0, y, z) == CUBE_AIR)
						facesRight.push_back({z, y, 1, 1, x + 1});
				}

				// Create face left
				if (x - 1 >= 0)
				{
					if (this->at(x - 1, y, z) == CUBE_AIR)
						facesLeft.push_back({z, y, 1, 1, x});
				}
				else if (leftChunk)
				{
					if (leftChunk->at(CHUNK_MAX, y, z) == CUBE_AIR)
						facesLeft.push_back({z, y, 1, 1, x});
				}

				// Create face up
				if (y + 1 < CHUNK_HEIGHT)
				{
					if (this->at(x, y + 1, z) == CUBE_AIR)
						facesUp.push_back({x, z, 1, 1, y + 1});
				}
				else
				{
					facesUp.push_back({x, z, 1, 1, y + 1});
				}

				// Create face down
				if (y - 1 >= 0)
				{
					if (this->at(x, y - 1, z) == CUBE_AIR)
						facesDown.push_back({x, z, 1, 1, y});
				}
				else
				{
					facesDown.push_back({x, z, 1, 1, y});
				}
			}
		}
	}

	gm::Vec3f	pointLU, pointLD, pointRD, pointRU;

	// Face front
	for (Face &f : facesFront)
	{
		pointLU = gm::Vec3f(f.x      , f.y + f.h, f.axis);
		pointLD = gm::Vec3f(f.x      , f.y      , f.axis);
		pointRD = gm::Vec3f(f.x + f.w, f.y      , f.axis);
		pointRU = gm::Vec3f(f.x + f.w, f.y + f.h, f.axis);
		createTriangleFace(vertexIndex, vertices, indices, nbVertex,
							pointLU, pointLD, pointRD, pointRU, normalFront);
	}

	// Face back
	for (Face &f : facesBack)
	{
		pointLU = gm::Vec3f(f.x + f.w, f.y + f.h, f.axis);
		pointLD = gm::Vec3f(f.x + f.w, f.y      , f.axis);
		pointRD = gm::Vec3f(f.x      , f.y      , f.axis);
		pointRU = gm::Vec3f(f.x      , f.y + f.h, f.axis);
		createTriangleFace(vertexIndex, vertices, indices, nbVertex,
							pointLU, pointLD, pointRD, pointRU, normalBack);
	}

	// Face right
	for (Face &f : facesRight)
	{
		pointLU = gm::Vec3f(f.axis, f.y + f.h, f.x + f.w);
		pointLD = gm::Vec3f(f.axis, f.y      , f.x + f.w);
		pointRD = gm::Vec3f(f.axis, f.y      , f.x      );
		pointRU = gm::Vec3f(f.axis, f.y + f.h, f.x      );
		createTriangleFace(vertexIndex, vertices, indices, nbVertex,
							pointLU, pointLD, pointRD, pointRU, normalRight);
	}

	// Face right
	for (Face &f : facesLeft)
	{
		pointLU = gm::Vec3f(f.axis, f.y + f.h, f.x      );
		pointLD = gm::Vec3f(f.axis, f.y      , f.x      );
		pointRD = gm::Vec3f(f.axis, f.y      , f.x + f.w);
		pointRU = gm::Vec3f(f.axis, f.y + f.h, f.x + f.w);
		createTriangleFace(vertexIndex, vertices, indices, nbVertex,
							pointLU, pointLD, pointRD, pointRU, normalLeft);
	}

	// Face up
	for (Face &f : facesUp)
	{
		pointLU = gm::Vec3f(f.x      , f.axis, f.y      );
		pointLD = gm::Vec3f(f.x      , f.axis, f.y + f.h);
		pointRD = gm::Vec3f(f.x + f.w, f.axis, f.y + f.h);
		pointRU = gm::Vec3f(f.x + f.w, f.axis, f.y      );
		createTriangleFace(vertexIndex, vertices, indices, nbVertex,
							pointLU, pointLD, pointRD, pointRU, normalUp);
	}

	// Face down
	for (Face &f : facesDown)
	{
		pointLU = gm::Vec3f(f.x      , f.axis, f.y + f.h);
		pointLD = gm::Vec3f(f.x      , f.axis, f.y      );
		pointRD = gm::Vec3f(f.x + f.w, f.axis, f.y      );
		pointRU = gm::Vec3f(f.x + f.w, f.axis, f.y + f.h);
		createTriangleFace(vertexIndex, vertices, indices, nbVertex,
							pointLU, pointLD, pointRD, pointRU, normalUp);
	}

	this->mesh = ChunkMesh(vertices, indices);
	this->mesh.createBuffers(*this->copyCommandPool);
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


static void	createTriangleFace(
				std::unordered_map<std::size_t, uint32_t> &vertexIndex,
				std::vector<VertexPosNrm> &vertices,
				std::vector<uint32_t> &indices,
				int &nbVertex,
				const gm::Vec3f &posLU,
				const gm::Vec3f &posLD,
				const gm::Vec3f &posRD,
				const gm::Vec3f &posRU,
				const gm::Vec3f &normal)
{
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
