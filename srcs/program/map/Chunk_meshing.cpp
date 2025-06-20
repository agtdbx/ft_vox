#include <program/map/Chunk.hpp>

#include <program/map/Map.hpp>
#include <program/bytes/bitFunctions.hpp>

#include <unordered_map>

//**** STATIC FUNCTIONS DEFINE *************************************************

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

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
//**** PUBLIC METHODS **********************************************************
//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Chunk::createBorderMesh(PerfLogger &perfLogger)
{
	perflogStart(perfLogger.meshChunk);

	std::vector<VertexPos>	&vertices = this->borderMesh.getVertices();
	std::vector<uint32_t>	&indices = this->borderMesh.getIndices();
	float	h0, h1;
	int		nbVertex = 0;

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
		indices.push_back(nbVertex    );indices.push_back(nbVertex + 1);indices.push_back(nbVertex + 2);
		indices.push_back(nbVertex    );indices.push_back(nbVertex + 2);indices.push_back(nbVertex + 3);
		// Back face
		indices.push_back(nbVertex + 6);indices.push_back(nbVertex + 7);indices.push_back(nbVertex + 5);
		indices.push_back(nbVertex + 6);indices.push_back(nbVertex + 5);indices.push_back(nbVertex + 4);
		// Left face
		indices.push_back(nbVertex + 4);indices.push_back(nbVertex + 5);indices.push_back(nbVertex + 1);
		indices.push_back(nbVertex + 4);indices.push_back(nbVertex + 1);indices.push_back(nbVertex    );
		// Right face
		indices.push_back(nbVertex + 2);indices.push_back(nbVertex + 3);indices.push_back(nbVertex + 7);
		indices.push_back(nbVertex + 2);indices.push_back(nbVertex + 7);indices.push_back(nbVertex + 6);

		nbVertex += 8;
	}

	perflogEnd(perfLogger.meshChunk);

	this->borderMesh.updateMeshInfo();
}


void	Chunk::createMesh(Map &map, PerfLogger &perfLogger)
{
	perflogStart(perfLogger.meshBlock);

	std::unordered_map<std::size_t, uint32_t>	vertexIndex;
	std::vector<VertexVoxel>					&vertices = this->mesh.getVertices();
	std::vector<uint32_t>						&indices = this->mesh.getIndices();
	int											nbVertex = 0;
	CubeBitmap	*frontBitmap = map.getChunkBitmap(this->chunkId.x, this->chunkId.y + 1);
	CubeBitmap	*backBitmap = map.getChunkBitmap(this->chunkId.x, this->chunkId.y - 1);
	CubeBitmap	*rightBitmap = map.getChunkBitmap(this->chunkId.x + 1, this->chunkId.y);
	CubeBitmap	*leftBitmap = map.getChunkBitmap(this->chunkId.x - 1, this->chunkId.y);

	int	idY;
	uint64_t	chunkLineL, chunkLineR;
	uint64_t	chunkLeftLine, chunkCurrLine, chunkRightLine;
	gm::Vec3f	pointLU, pointLD, pointRD, pointRU;

	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		idY = y * CHUNK_SIZE;

		// y axis
		perflogStart(perfLogger.meshBlockYaxis);
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				const Cube	&type = this->at(x, y, z);
				if (type == CUBE_AIR || type == CUBE_WATER)
					continue;

				// Face up
				if (y == CHUNK_MAX_H || this->cubeBitmap.getX(x, y + 1, z) == 0)
				{
					pointLU = gm::Vec3f(x    , y + 1, z    );
					pointLD = gm::Vec3f(x    , y + 1, z + 1);
					pointRD = gm::Vec3f(x + 1, y + 1, z + 1);
					pointRU = gm::Vec3f(x + 1, y + 1, z    );
					createTriangleFace(vertexIndex, vertices, indices, nbVertex,
										pointLU, pointLD, pointRD, pointRU, normalUp, type);
				}

				// Face down
				if (y == 0 || this->cubeBitmap.getX(x, y - 1, z) == 0)
				{
					pointLU = gm::Vec3f(x    , y    , z + 1);
					pointLD = gm::Vec3f(x    , y    , z    );
					pointRD = gm::Vec3f(x + 1, y    , z    );
					pointRU = gm::Vec3f(x + 1, y    , z + 1);
					createTriangleFace(vertexIndex, vertices, indices, nbVertex,
										pointLU, pointLD, pointRD, pointRU, normalDown, type);
				}
			}
		}
		perflogEnd(perfLogger.meshBlockYaxis);

		// z axis
		perflogStart(perfLogger.meshBlockZaxis);
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			// Contruct chunk line
			if (backBitmap)
				chunkLeftLine = ((backBitmap->axisZ[x + idY] >> CHUNK_MAX) & 1) & UINT32_MAX;
			else
				chunkLeftLine = 0ull;

			chunkCurrLine = this->cubeBitmap.axisZ[x + idY] & UINT32_MAX;

			if (frontBitmap)
				chunkRightLine = ((frontBitmap->axisZ[x + idY] >> 0) & 1) & UINT32_MAX;
			else
				chunkRightLine = 0ull;

			chunkLineL = (chunkRightLine << 33) | (chunkCurrLine << 1) | chunkLeftLine;
			chunkLineR = reverseBytes(chunkLineL) >> 30;

			// Face front
			while (chunkLineR != 0)
			{
				int z = trailingZero(chunkLineR);
				int	length = trailingOne(chunkLineR >> z);

				chunkLineR -= createLengthMask(length) << z;

				if (z == 0 || z > CHUNK_SIZE)
					continue;
				z = CHUNK_MAX - (z - 1);

				const Cube	&type = this->at(x, y, z);
				pointLU = gm::Vec3f(x    , y + 1, z + 1);
				pointLD = gm::Vec3f(x    , y    , z + 1);
				pointRD = gm::Vec3f(x + 1, y    , z + 1);
				pointRU = gm::Vec3f(x + 1, y + 1, z + 1);
				createTriangleFace(vertexIndex, vertices, indices, nbVertex,
									pointLU, pointLD, pointRD, pointRU, normalFront, type);
			}

			// Face back
			while (chunkLineL != 0)
			{
				int z = trailingZero(chunkLineL);
				int	length = trailingOne(chunkLineL >> z);

				chunkLineL -= createLengthMask(length) << z;

				if (z == 0 || z > CHUNK_SIZE)
					continue;
				z--;

				const Cube	&type = this->at(x, y, z);
				pointLU = gm::Vec3f(x + 1, y + 1, z    );
				pointLD = gm::Vec3f(x + 1, y    , z    );
				pointRD = gm::Vec3f(x    , y    , z    );
				pointRU = gm::Vec3f(x    , y + 1, z    );
				createTriangleFace(vertexIndex, vertices, indices, nbVertex,
									pointLU, pointLD, pointRD, pointRU, normalBack, type);
			}
		}
		perflogEnd(perfLogger.meshBlockZaxis);

		// x axis
		perflogStart(perfLogger.meshBlockXaxis);
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			// Contruct chunk line
			if (leftBitmap)
				chunkLeftLine = ((leftBitmap->axisX[z + idY] >> CHUNK_MAX) & 1) & UINT32_MAX;
			else
				chunkLeftLine = 0ull;

			chunkCurrLine = this->cubeBitmap.axisX[z + idY] & UINT32_MAX;

			if (rightBitmap)
				chunkRightLine = ((rightBitmap->axisX[z + idY] >> 0) & 1) & UINT32_MAX;
			else
				chunkRightLine = 0ull;

			chunkLineL = (chunkRightLine << 33) | (chunkCurrLine << 1) | chunkLeftLine;
			chunkLineR = reverseBytes(chunkLineL) >> 30;

			// Face right
			while (chunkLineR != 0)
			{
				int x = trailingZero(chunkLineR);
				int	length = trailingOne(chunkLineR >> x);

				chunkLineR -= createLengthMask(length) << x;

				if (x == 0 || x > CHUNK_SIZE)
					continue;
				x = CHUNK_MAX - (x - 1);

				const Cube	&type = this->at(x, y, z);
				pointLU = gm::Vec3f(x + 1, y + 1, z + 1);
				pointLD = gm::Vec3f(x + 1, y    , z + 1);
				pointRD = gm::Vec3f(x + 1, y    , z    );
				pointRU = gm::Vec3f(x + 1, y + 1, z    );
				createTriangleFace(vertexIndex, vertices, indices, nbVertex,
									pointLU, pointLD, pointRD, pointRU, normalRight, type);
			}

			// Face left
			while (chunkLineL != 0)
			{
				int x = trailingZero(chunkLineL);
				int	length = trailingOne(chunkLineL >> x);

				chunkLineL -= createLengthMask(length) << x;

				if (x == 0 || x > CHUNK_SIZE)
					continue;
				x--;

				const Cube	&type = this->at(x, y, z);
				pointLU = gm::Vec3f(x    , y + 1, z    );
				pointLD = gm::Vec3f(x    , y    , z    );
				pointRD = gm::Vec3f(x    , y    , z + 1);
				pointRU = gm::Vec3f(x    , y + 1, z + 1);
				createTriangleFace(vertexIndex, vertices, indices, nbVertex,
									pointLU, pointLD, pointRD, pointRU, normalLeft, type);
			}
		}
		perflogEnd(perfLogger.meshBlockXaxis);
	}

	perflogEnd(perfLogger.meshBlock);

	this->mesh.updateMeshInfo();
}


void	Chunk::createWaterMesh(PerfLogger &perfLogger)
{
	perflogStart(perfLogger.meshWater);

	std::unordered_map<std::size_t, uint32_t>	vertexIndex;
	std::vector<VertexVoxel>					&vertices = this->waterMesh.getVertices();
	std::vector<uint32_t>						&indices = this->waterMesh.getIndices();
	int											nbVertex = 0;

	gm::Vec3f	pointLU, pointLD, pointRD, pointRU;

	int	x, w;
	int	y = CHUNK_WATER_LEVEL;
	for (int z = 0; z < CHUNK_SIZE; z++)
	{
		x = 0;
		while (x < CHUNK_SIZE)
		{
			if (this->at(x, y, z) != CUBE_WATER)
			{
				x++;
				continue;
			}

			w = 1;
			while (x + w < CHUNK_SIZE)
			{
				if (this->at(x + w, y, z) != CUBE_WATER)
					break;
				w++;
			}

			// Face up
			pointLU = gm::Vec3f(x    , y + 1, z    );
			pointLD = gm::Vec3f(x    , y + 1, z + 1);
			pointRD = gm::Vec3f(x + w, y + 1, z + 1);
			pointRU = gm::Vec3f(x + w, y + 1, z    );
			createTriangleFace(vertexIndex, vertices, indices, nbVertex,
								pointLU, pointLD, pointRD, pointRU, normalUp, CUBE_WATER);

			x += w;
		}
	}

	perflogEnd(perfLogger.meshWater);

	this->waterMesh.updateMeshInfo();
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
