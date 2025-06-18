#include <program/map/Chunk.hpp>

#include <program/map/Map.hpp>

#include <unordered_map>

PerlinNoise PerlinGeration(42, 64);
PerlinNoise PerlinTerrain(854, 2);
PerlinNoise PerlinBiome(654, 4096);
//PerlinNoise PerlinCave(8576, 128);

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
					std::vector<VertexVoxel> &vertices,
					VertexVoxel &vertex,
					int &nbVertex);
static void	createTriangleFace(
				std::unordered_map<std::size_t, uint32_t> &vertexIndex,
				std::vector<VertexVoxel> &vertices,
				std::vector<uint32_t> &indices,
				int &nbVertex,
				const gm::Vec3f &posLU,
				const gm::Vec3f &posLD,
				const gm::Vec3f &posRD,
				const gm::Vec3f &posRU,
				const gm::Vec3f &normal,
				const Cube &type);
static	bool	isCubeTransparent(const Cube &cube);
// static bool	getCubeX(int32_t cubesBitmap[CHUNK_MASK_SIZE], int x, int y, int z);
// static bool	getCubeZ(int32_t cubesBitmap[CHUNK_MASK_SIZE], int x, int y, int z);
// static void	setCubeX(int32_t cubesBitmap[CHUNK_MASK_SIZE], int x, int y, int z, bool cube);
// static void	setCubeZ(int32_t cubesBitmap[CHUNK_MASK_SIZE], int x, int y, int z, bool cube);
// static int32_t	createLengthMask(int length);

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
		this->cubesBitmapX[i] = 0;
		this->cubesBitmapZ[i] = 0;
	}
	this->copyCommandPool = NULL;

	int	halfChunkSize = CHUNK_SIZE / 2;
	int	halfChunkHeight = CHUNK_HEIGHT / 2;
	this->boundingCube.center = gm::Vec3f(halfChunkSize, halfChunkHeight, halfChunkSize);
	this->boundingCube.extents = gm::Vec3f(halfChunkSize, halfChunkHeight, halfChunkSize);
}


Chunk::Chunk(const Chunk &obj)
{
	this->chunkId = obj.chunkId;
	this->chunkPosition = obj.chunkPosition;
	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
		this->cubes[i] = obj.cubes[i];
	for (int i = 0; i < CHUNK_MASK_SIZE; i++)
	{
		this->cubesBitmapX[i] = obj.cubesBitmapX[i];
		this->cubesBitmapZ[i] = obj.cubesBitmapZ[i];
	}
	this->copyCommandPool = obj.copyCommandPool;
	this->mesh = obj.mesh;
	this->boundingCube = obj.boundingCube;
}

//---- Destructor --------------------------------------------------------------

Chunk::~Chunk()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

const BoundingCube	&Chunk::getBoundingCube(void) const
{
	return (this->boundingCube);
}


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
		this->cubesBitmapX[i] = obj.cubesBitmapX[i];
		this->cubesBitmapZ[i] = obj.cubesBitmapZ[i];
	}

	this->boundingCube = obj.boundingCube;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Chunk::init(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader)
{
	this->copyCommandPool = &engine.commandPool;

	chunkShader.shader.initShaderParam(engine, this->shaderParam, {"cubes"});
	chunkShader.shaderWater.initShaderParam(engine, this->shaderParamWater, {"cubes"});
	chunkShader.shaderFdf.initShaderParam(engine, this->shaderParamFdf, {});
	chunkShader.shaderFdf.initShaderParam(engine, this->shaderParamFdfWater, {});
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

	this->boundingCube.center = this->chunkPosition;

	float maxSize = 0;
	float perlinX = 0;
	float perlinZ = 0;
	float tmpX = 0;
	float tmpZ = 0;
	float Biome = 0;
	float Moutain = 0;
	int		idZ, id, idBitmapY, maskX, maskZ;
	//float Cave = 0;
	for (int z = 0; z < CHUNK_SIZE; z++)
	{
		maskZ = (0b1 << z);
		idZ = z * CHUNK_SIZE;
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			maskX = (0b1 << x);
			tmpX = ((int)this->chunkPosition.x + x) % MAP_SIZE;
			if (tmpX < 0)
				tmpX += MAP_SIZE;

			tmpZ = ((int)this->chunkPosition.z + z) % MAP_SIZE;
			if (tmpZ < 0)
				tmpZ += MAP_SIZE;
			// Begin

			perlinX = (float)tmpX / (float)MAP_SIZE;
			perlinZ = (float)tmpZ / (float)MAP_SIZE;

			//TODO Moutain will need a lot tweaking (Biome need more chunk to see if he is garbage)
			maxSize = (PerlinGeration.getNoise(perlinX, perlinZ) * 32.0f + 48.0f);
			Biome = (PerlinBiome.getNoise(perlinX, perlinZ));
			Moutain = (PerlinTerrain.getNoise(perlinX, perlinZ));
			// faire des truc aprés sa
			//Cave = (PerlinCave.getNoise(perlinX, perlinZ));
			if (Moutain < 0.2)
				maxSize = maxSize - (Moutain * 32.0f + 48.0f);
			else if (Moutain > 0.8)
				maxSize = maxSize + (Moutain * 32.0f + 48.0f);
			if (maxSize < 0)
				maxSize = 1;
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				//with this setup stone cannot be seen on the surface
				id = x + idZ + y * CHUNK_SIZE2;
				if (y > (int)maxSize && y > 58)
					break;

				if (y > (int)maxSize && y <= 58)
				{
					if (Biome > 0.7)
						this->cubes[id] = CUBE_LAVA; // TODO : Make lava transparent
					else if (Biome < 0.3)
						this->cubes[id] = CUBE_ICE;
					else
					{
						this->cubes[id] = CUBE_WATER;
						continue;
					}
					idBitmapY = y * CHUNK_SIZE;
					this->cubesBitmapX[z + idBitmapY] += maskX;
					this->cubesBitmapZ[x + idBitmapY] += maskZ;
			}
				// End
				else
				{
					idBitmapY = y * CHUNK_SIZE;
					this->cubesBitmapX[z + idBitmapY] += maskX;
					this->cubesBitmapZ[x + idBitmapY] += maskZ;
					if (y > 57 && y == (int)maxSize)
					{
						if (Biome > 0.7)
							this->cubes[id] = CUBE_SAND;
						else if (Biome < 0.3)
							this->cubes[id] = CUBE_SNOW;
						else
							this->cubes[id] = CUBE_GRASS;
					}
					else if (y > 58 && y > (int)maxSize - 3 && y < (int)maxSize)
						this->cubes[id] = CUBE_DIRT;
					else if (y != 0)
						this->cubes[id] = CUBE_STONE;
					else
						this->cubes[id] = CUBE_DIAMOND;
				}
			}
		}
	}
}


void	Chunk::createMeshes(Map &map)
{
	this->createMesh(map);
	this->createWaterMesh();

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
		this->shaderParam.updateBuffer(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->mesh, chunkShader.shader, this->shaderParam);
	}
	else
	{
		this->shaderParamFdf.updateBuffer(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->mesh, chunkShader.shaderFdf, this->shaderParamFdf);
	}

	if (chunkShader.shaderBorderEnable)
	{
		this->shaderParamBorder.updateBuffer(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->borderMesh, chunkShader.shaderBorder, this->shaderParamBorder);
	}
}


void	Chunk::drawWater(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	if (this->waterMesh.getNbIndex() == 0)
		return ;

	// Draw mesh
	if (!chunkShader.shaderFdfEnable)
	{
		this->shaderParamWater.updateBuffer(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->waterMesh, chunkShader.shaderWater, this->shaderParamWater);
	}
	else
	{
		this->shaderParamFdfWater.updateBuffer(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->waterMesh, chunkShader.shaderFdf, this->shaderParamFdfWater);
	}
}


void	Chunk::destroy(Engine &engine)
{
	this->shaderParam.destroy(engine);
	this->shaderParamWater.destroy(engine);
	this->shaderParamFdf.destroy(engine);
	this->shaderParamFdfWater.destroy(engine);
	this->mesh.destroy();
	this->waterMesh.destroy();
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
	std::vector<VertexVoxel>					vertices;
	std::vector<uint32_t>						indices;
	int											nbVertex = 0;
	Chunk	*leftChunk = map.getChunk(this->chunkId.x - 1, this->chunkId.y);
	Chunk	*rightChunk = map.getChunk(this->chunkId.x + 1, this->chunkId.y);
	Chunk	*frontChunk = map.getChunk(this->chunkId.x, this->chunkId.y + 1);
	Chunk	*backChunk = map.getChunk(this->chunkId.x, this->chunkId.y - 1);

	gm::Vec3f	pointLU, pointLD, pointRD, pointRU;

	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				const Cube	&type = this->at(x, y, z);
				if (type == CUBE_AIR || type == CUBE_WATER)
					continue;

				// Face up
				if (y == CHUNK_MAX_H || isCubeTransparent(this->at(x, y + 1, z)))
				{
					pointLU = gm::Vec3f(x + 0, y + 1, z + 0);
					pointLD = gm::Vec3f(x + 0, y + 1, z + 1);
					pointRD = gm::Vec3f(x + 1, y + 1, z + 1);
					pointRU = gm::Vec3f(x + 1, y + 1, z + 0);
					createTriangleFace(vertexIndex, vertices, indices, nbVertex,
										pointLU, pointLD, pointRD, pointRU, normalUp, type);
				}

				// Face down
				if (y == 0 || isCubeTransparent(this->at(x, y - 1, z)))
				{
					pointLU = gm::Vec3f(x + 0, y + 0, z + 1);
					pointLD = gm::Vec3f(x + 0, y + 0, z + 0);
					pointRD = gm::Vec3f(x + 1, y + 0, z + 0);
					pointRU = gm::Vec3f(x + 1, y + 0, z + 1);
					createTriangleFace(vertexIndex, vertices, indices, nbVertex,
										pointLU, pointLD, pointRD, pointRU, normalDown, type);
				}

				// Face front
				if ((z != CHUNK_MAX && isCubeTransparent(this->at(x, y, z + 1))) ||
					(z == CHUNK_MAX && isCubeTransparent(frontChunk->at(x, y, 0))))
				{
					pointLU = gm::Vec3f(x + 0, y + 1, z + 1);
					pointLD = gm::Vec3f(x + 0, y + 0, z + 1);
					pointRD = gm::Vec3f(x + 1, y + 0, z + 1);
					pointRU = gm::Vec3f(x + 1, y + 1, z + 1);
					createTriangleFace(vertexIndex, vertices, indices, nbVertex,
										pointLU, pointLD, pointRD, pointRU, normalFront, type);
				}

				// Face back
				if ((z != 0 && isCubeTransparent(this->at(x, y, z - 1))) ||
					(z == 0 && isCubeTransparent(backChunk->at(x, y, CHUNK_MAX))))
				{
					pointLU = gm::Vec3f(x + 1, y + 1, z + 0);
					pointLD = gm::Vec3f(x + 1, y + 0, z + 0);
					pointRD = gm::Vec3f(x + 0, y + 0, z + 0);
					pointRU = gm::Vec3f(x + 0, y + 1, z + 0);
					createTriangleFace(vertexIndex, vertices, indices, nbVertex,
										pointLU, pointLD, pointRD, pointRU, normalBack, type);
				}

				// Face right
				if ((x != CHUNK_MAX && isCubeTransparent(this->at(x + 1, y, z))) ||
					(x == CHUNK_MAX && isCubeTransparent(rightChunk->at(0, y, z))))
				{
					pointLU = gm::Vec3f(x + 1, y + 1, z + 1);
					pointLD = gm::Vec3f(x + 1, y + 0, z + 1);
					pointRD = gm::Vec3f(x + 1, y + 0, z + 0);
					pointRU = gm::Vec3f(x + 1, y + 1, z + 0);
					createTriangleFace(vertexIndex, vertices, indices, nbVertex,
										pointLU, pointLD, pointRD, pointRU, normalRight, type);
				}

				// Face left
				if ((x != 0 && isCubeTransparent(this->at(x - 1, y, z))) ||
					(x == 0 && isCubeTransparent(leftChunk->at(CHUNK_MAX, y, z))))
				{
					pointLU = gm::Vec3f(x + 0, y + 1, z + 0);
					pointLD = gm::Vec3f(x + 0, y + 0, z + 0);
					pointRD = gm::Vec3f(x + 0, y + 0, z + 1);
					pointRU = gm::Vec3f(x + 0, y + 1, z + 1);
					createTriangleFace(vertexIndex, vertices, indices, nbVertex,
										pointLU, pointLD, pointRD, pointRU, normalLeft, type);
				}
			}
		}
	}

	this->mesh = ChunkMesh(vertices, indices);
	this->mesh.createBuffers(*this->copyCommandPool);
}


void	Chunk::createWaterMesh(void)
{
	std::unordered_map<std::size_t, uint32_t>	vertexIndex;
	std::vector<VertexVoxel>					vertices;
	std::vector<uint32_t>						indices;
	int											nbVertex = 0;

	gm::Vec3f	pointLU, pointLD, pointRD, pointRU;

	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				const Cube	&type = this->at(x, y, z);
				if (type != CUBE_WATER)
					continue;

				// Face up
				if (y == CHUNK_MAX_H || this->at(x, y + 1, z) == CUBE_AIR)
				{
					pointLU = gm::Vec3f(x + 0, y + 1, z + 0);
					pointLD = gm::Vec3f(x + 0, y + 1, z + 1);
					pointRD = gm::Vec3f(x + 1, y + 1, z + 1);
					pointRU = gm::Vec3f(x + 1, y + 1, z + 0);
					createTriangleFace(vertexIndex, vertices, indices, nbVertex,
										pointLU, pointLD, pointRD, pointRU, normalUp, type);
				}
			}
		}
	}

	this->waterMesh = ChunkMesh(vertices, indices);
	this->waterMesh.createBuffers(*this->copyCommandPool);
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static uint32_t	getVetrexId(
					std::unordered_map<std::size_t, uint32_t> &vertexIndex,
					std::vector<VertexVoxel> &vertices,
					VertexVoxel &vertex,
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
				std::vector<VertexVoxel> &vertices,
				std::vector<uint32_t> &indices,
				int &nbVertex,
				const gm::Vec3f &posLU,
				const gm::Vec3f &posLD,
				const gm::Vec3f &posRD,
				const gm::Vec3f &posRU,
				const gm::Vec3f &normal,
				const Cube &type)
{
	VertexVoxel pointLU(posLU, normal, type);
	VertexVoxel pointLD(posLD, normal, type);
	VertexVoxel pointRD(posRD, normal, type);
	VertexVoxel pointRU(posRU, normal, type);

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


static	bool	isCubeTransparent(const Cube &cube)
{
	return (cube == CUBE_AIR || cube == CUBE_WATER);
}


// static bool	getCubeX(int32_t cubesBitmap[CHUNK_MASK_SIZE], int x, int y, int z)
// {
// 	return (cubesBitmap[z + y * CHUNK_SIZE] & (0b1 << x));
// }


// static bool	getCubeZ(int32_t cubesBitmap[CHUNK_MASK_SIZE], int x, int y, int z)
// {
// 	return (cubesBitmap[x + y * CHUNK_SIZE] & (0b1 << z));
// }


// static void	setCubeX(int32_t cubesBitmap[CHUNK_MASK_SIZE], int x, int y, int z, bool cube)
// {
// 	int		id = z + y * CHUNK_SIZE;
// 	int32_t	mask = 0b1 << x;
// 	if ((cubesBitmap[id] & mask) == cube)
// 		return ;

// 	if (cube)
// 		cubesBitmap[id] += mask;
// 	else
// 		cubesBitmap[id] -= mask;
// }


// static void	setCubeZ(int32_t cubesBitmap[CHUNK_MASK_SIZE], int x, int y, int z, bool cube)
// {
// 	int		id = x + y * CHUNK_SIZE;
// 	int32_t	mask = 0b1 << z;
// 	if ((cubesBitmap[id] & mask) == cube)
// 		return ;

// 	if (cube)
// 		cubesBitmap[id] += mask;
// 	else
// 		cubesBitmap[id] -= mask;
// }


// static int32_t	createLengthMask(int length)
// {
// 	int32_t	res = 0;

// 	for (int i = 0; i < length; i++)
// 		res += (0b1 << i);

// 	return (res);
// }
