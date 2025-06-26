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
const uint256_t	zero256;
const uint256_t	one256(1ull);

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

	int	idY, idZ;
	uint64_t	chunkLineL, chunkLineR, chunkLineTmp, cubeMask, airMask;
	uint256_t	chunkLineU, chunkLineD, chunkLineTmpY, cubeMask256, airMask256;
	uint64_t	chunkLeftLine, chunkRightLine;
	uint64_t	chunkLeftBlock, chunkCurrLine, chunkRightBlock;
	gm::Vec3f	pointLU, pointLD, pointRD, pointRU;

	perflogStart(perfLogger.meshBlockCopyBitmap);
	CubeBitmap	cBitmapL = this->cubeBitmap;
	CubeBitmap	cBitmapR = this->cubeBitmap;

	for (int i = 0; i < CHUNK_MASK_SIZE; i++)
	{
		cBitmapR.axisX[i] = reverse32Bytes(cBitmapR.axisX[i]);
		cBitmapR.axisZ[i] = reverse32Bytes(cBitmapR.axisZ[i]);
	}

	for (int i = 0; i < CHUNK_SIZE2; i++)
	{
		cBitmapR.axisY[i] = reverse256Bytes(cBitmapR.axisY[i]);
	}
	perflogEnd(perfLogger.meshBlockCopyBitmap);

	// y axis
	perflogStart(perfLogger.meshBlockYaxis);
	for (int z = 0; z < CHUNK_SIZE; z++)
	{
		idZ = z * CHUNK_SIZE;
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			// Face up
			chunkLineU = cBitmapR.axisY[x + idZ];
			while (chunkLineU != zero256)
			{
				// Get face position
				int y = trailing256Zero(chunkLineU);
				int	length = trailing256One(chunkLineU >> y);

				chunkLineU -= create256LengthMask(length) << y;

				if (y >= CHUNK_HEIGHT)
					continue;

				// Create masks
				cubeMask256 = one256 << y;
				if (y == 0)
					airMask256 = zero256;
				else
					airMask256 = one256 << (y - 1);

				// Get real y and cube type
				int	yShift = y;
				y = CHUNK_MAX_H - y;
				const Cube	&type = this->at(x, y, z);

				// Extend in x axis
				int w = 1;
				while (x + w < CHUNK_SIZE)
				{
					chunkLineTmpY = cBitmapR.axisY[(x + w) + idZ];

					if ((chunkLineTmpY & cubeMask256) == zero256)
						break;

					if ((chunkLineTmpY & airMask256) != zero256)
						break;

					if (this->at(x + w, y, z) != type)
						break;

					length = trailing256One(chunkLineTmpY >> yShift);
					cBitmapR.axisY[(x + w) + idZ] -= create256LengthMask(length) << yShift;
					w++;
				}

				// Extend in z axis
				int d = 1;
				while (z + d < CHUNK_SIZE)
				{
					int idZ2 = (z + d) * CHUNK_SIZE;
					int tmpW = 0;
					while (tmpW < w)
					{
						chunkLineTmpY = cBitmapR.axisY[(x + tmpW) + idZ2];

						if ((chunkLineTmpY & cubeMask256) == zero256)
							break;

						if ((chunkLineTmpY & airMask256) != zero256)
							break;

						if (this->at(x + tmpW, y, z + d) != type)
							break;

						tmpW++;
					}

					if (tmpW != w)
						break;

					for (int i = 0; i < w; i++)
					{
						length = trailing256One(cBitmapR.axisY[(x + i) + idZ2] >> yShift);
						cBitmapR.axisY[(x + i) + idZ2] -= create256LengthMask(length) << yShift;
					}
					d++;
				}

				pointLU = gm::Vec3f(x    , y + 1, z    );
				pointLD = gm::Vec3f(x    , y + 1, z + d);
				pointRD = gm::Vec3f(x + w, y + 1, z + d);
				pointRU = gm::Vec3f(x + w, y + 1, z    );
				createTriangleFace(vertexIndex, vertices, indices, nbVertex,
									pointLU, pointLD, pointRD, pointRU, normalUp, type);
			}

			// Face down
			chunkLineD = cBitmapL.axisY[x + idZ];
			while (chunkLineD != zero256)
			{
				int y = trailing256Zero(chunkLineD);
				int	length = trailing256One(chunkLineD >> y);

				chunkLineD -= create256LengthMask(length) << y;

				if (y >= CHUNK_HEIGHT)
					continue;

				// Create masks
				cubeMask256 = one256 << y;
				if (y == 0)
					airMask256 = zero256;
				else
					airMask256 = one256 << (y - 1);

				// Get cube type
				const Cube	&type = this->at(x, y, z);

				// Extend in x axis
				int w = 1;
				while (x + w < CHUNK_SIZE)
				{
					chunkLineTmpY = cBitmapL.axisY[(x + w) + idZ];

					if ((chunkLineTmpY & cubeMask256) == zero256)
						break;

					if ((chunkLineTmpY & airMask256) != zero256)
						break;

					if (this->at(x + w, y, z) != type)
						break;

					length = trailing256One(chunkLineTmpY >> y);
					cBitmapL.axisY[(x + w) + idZ] -= create256LengthMask(length) << y;
					w++;
				}

				// Extend in z axis
				int	d = 1;
				while (z + d < CHUNK_SIZE)
				{
					int idZ2 = (z + d) * CHUNK_SIZE;
					int tmpW = 0;
					while (tmpW < w)
					{
						chunkLineTmpY = cBitmapL.axisY[(x + tmpW) + idZ2];

						if ((chunkLineTmpY & cubeMask256) == zero256)
							break;

						if ((chunkLineTmpY & airMask256) != zero256)
							break;

						if (this->at(x + tmpW, y, z + d) != type)
							break;

						tmpW++;
					}

					if (tmpW != w)
						break;

					for (int i = 0; i < w; i++)
					{
						length = trailing256One(cBitmapL.axisY[(x + i) + idZ2] >> y);
						cBitmapL.axisY[(x + i) + idZ2] -= create256LengthMask(length) << y;
					}
					d++;
				}

				// Create face
				pointLU = gm::Vec3f(x    , y    , z + d);
				pointLD = gm::Vec3f(x    , y    , z    );
				pointRD = gm::Vec3f(x + w, y    , z    );
				pointRU = gm::Vec3f(x + w, y    , z + d);
				createTriangleFace(vertexIndex, vertices, indices, nbVertex,
									pointLU, pointLD, pointRD, pointRU, normalDown, type);
			}
		}
	}
	perflogEnd(perfLogger.meshBlockYaxis);


	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		idY = y * CHUNK_SIZE;

		// z axis
		perflogStart(perfLogger.meshBlockZaxis);
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			// Face front
			if (backBitmap)
				chunkLeftLine = backBitmap->axisX[CHUNK_MAX + idY];
			else
				chunkLeftLine = 0ull;

			if (frontBitmap)
				chunkRightLine = frontBitmap->axisX[0 + idY];
			else
				chunkRightLine = 0ull;

			chunkLeftBlock = ((chunkLeftLine >> x) & 1) & UINT32_MAX;
			chunkRightBlock = ((chunkRightLine >> x) & 1) & UINT32_MAX;
			chunkCurrLine = cBitmapR.axisZ[x + idY] & UINT32_MAX;
			chunkLineR = (chunkLeftBlock << 33) | (chunkCurrLine << 1) | chunkRightBlock;

			while (chunkLineR != 0)
			{
				int z = trailing64Zero(chunkLineR);
				int	length = trailing64One(chunkLineR >> z);

				chunkLineR -= create64LengthMask(length) << z;

				if (z == 0 || z > CHUNK_SIZE)
					continue;

				// Create masks
				cubeMask = 1ull << z;
				airMask = 1ull << (z - 1);

				// Get real z and cube type
				int	zShift = z;
				z = CHUNK_MAX - (z - 1);
				const Cube	&type = this->at(x, y, z);

				// Extend in x axis
				int w = 1;
				while (x + w < CHUNK_SIZE)
				{
					chunkLeftBlock = ((chunkLeftLine >> (x + w)) & 1) & UINT32_MAX;
					chunkRightBlock = ((chunkRightLine >> (x + w)) & 1) & UINT32_MAX;
					chunkCurrLine = cBitmapR.axisZ[(x + w) + idY] & UINT32_MAX;
					chunkLineTmp = (chunkLeftBlock << 33) | (chunkCurrLine << 1) | chunkRightBlock;

					if ((chunkLineTmp & cubeMask) == 0ull)
						break;

					if ((chunkLineTmp & airMask) != 0ull)
						break;

					if (this->at(x + w, y, z) != type)
						break;

					length = trailing64One(chunkLineTmp >> zShift);
					cBitmapR.axisZ[(x + w) + idY] -= (create64LengthMask(length) << (zShift - 1)) & UINT32_MAX;
					w++;
				}

				// Extend in y axis
				int	h = 1;
				while (y + h < CHUNK_HEIGHT)
				{
					int	idY2 = (y + h) * CHUNK_SIZE;
					int	tmpX = 0;
					uint32_t	chunkLeftLine2 = 0ull;
					uint32_t	chunkRightLine2 = 0ull;

					if (backBitmap)
						chunkLeftLine2 = backBitmap->axisX[CHUNK_MAX + idY2];
					if (frontBitmap)
						chunkRightLine2 = frontBitmap->axisX[0 + idY2];

					while (tmpX < w)
					{
						chunkLeftBlock = ((chunkLeftLine2 >> (x + tmpX)) & 1) & UINT32_MAX;
						chunkRightBlock = ((chunkRightLine2 >> (x + tmpX)) & 1) & UINT32_MAX;
						chunkCurrLine = cBitmapR.axisZ[(x + tmpX) + idY2] & UINT32_MAX;
						chunkLineTmp = (chunkLeftBlock << 33) | (chunkCurrLine << 1) | chunkRightBlock;

						if ((chunkLineTmp & cubeMask) == 0ull)
							break;

						if ((chunkLineTmp & airMask) != 0ull)
							break;

						if (this->at(x + tmpX, y + h, z) != type)
							break;

						tmpX++;
					}

					if (tmpX != w)
						break;

					for (int i = 0; i < w; i++)
					{
						chunkLeftBlock = ((chunkLeftLine2 >> (x + i)) & 1) & UINT32_MAX;
						chunkRightBlock = ((chunkRightLine2 >> (x + i)) & 1) & UINT32_MAX;

						chunkCurrLine = cBitmapR.axisZ[(x + i) + idY2] & UINT32_MAX;
						chunkLineTmp = (chunkLeftBlock << 33) | (chunkCurrLine << 1) | chunkRightBlock;

						length = trailing64One(chunkLineTmp >> zShift);
						cBitmapR.axisZ[(x + i) + idY2] -= (create64LengthMask(length) << (zShift - 1)) & UINT32_MAX;
					}
					h++;
				}

				// Create face
				pointLU = gm::Vec3f(x    , y + h, z + 1);
				pointLD = gm::Vec3f(x    , y    , z + 1);
				pointRD = gm::Vec3f(x + w, y    , z + 1);
				pointRU = gm::Vec3f(x + w, y + h, z + 1);
				createTriangleFace(vertexIndex, vertices, indices, nbVertex,
									pointLU, pointLD, pointRD, pointRU, normalFront, type);
			}

			// Face back
			chunkCurrLine = cBitmapL.axisZ[x + idY] & UINT32_MAX;
			chunkLineL = (chunkRightBlock << 33) | (chunkCurrLine << 1) | chunkLeftBlock;

			while (chunkLineL != 0)
			{
				int z = trailing64Zero(chunkLineL);
				int	length = trailing64One(chunkLineL >> z);

				chunkLineL -= create64LengthMask(length) << z;

				if (z == 0 || z > CHUNK_SIZE)
					continue;

				// Create masks
				cubeMask = 1ull << z;
				airMask = 1ull << (z - 1);

				// Get real z and cube type
				int	zShift = z;
				z--;
				const Cube	&type = this->at(x, y, z);

				// Extend in x axis
				int w = 1;
				while (x + w < CHUNK_SIZE)
				{
					chunkLeftBlock = ((chunkLeftLine >> (x + w)) & 1) & UINT32_MAX;
					chunkRightBlock = ((chunkRightLine >> (x + w)) & 1) & UINT32_MAX;
					chunkCurrLine = cBitmapL.axisZ[(x + w) + idY] & UINT32_MAX;
					chunkLineTmp = (chunkRightBlock << 33) | (chunkCurrLine << 1) | chunkLeftBlock;

					if ((chunkLineTmp & cubeMask) == 0ull)
						break;

					if ((chunkLineTmp & airMask) != 0ull)
						break;

					if (this->at(x + w, y, z) != type)
						break;

					length = trailing64One(chunkLineTmp >> zShift);
					cBitmapL.axisZ[(x + w) + idY] -= (create64LengthMask(length) << (zShift - 1)) & UINT32_MAX;
					w++;
				}

				// Extend in y axis
				int	h = 1;
				while (y + h < CHUNK_HEIGHT)
				{
					int	idY2 = (y + h) * CHUNK_SIZE;
					int	tmpX = 0;
					uint32_t	chunkLeftLine2 = 0ull;
					uint32_t	chunkRightLine2 = 0ull;

					if (backBitmap)
						chunkLeftLine2 = backBitmap->axisX[CHUNK_MAX + idY2];
					if (frontBitmap)
						chunkRightLine2 = frontBitmap->axisX[0 + idY2];

					while (tmpX < w)
					{
						chunkLeftBlock = ((chunkLeftLine2 >> (x + tmpX)) & 1) & UINT32_MAX;
						chunkRightBlock = ((chunkRightLine2 >> (x + tmpX)) & 1) & UINT32_MAX;
						chunkCurrLine = cBitmapL.axisZ[(x + tmpX) + idY2] & UINT32_MAX;
						chunkLineTmp = (chunkRightBlock << 33) | (chunkCurrLine << 1) | chunkLeftBlock;

						if ((chunkLineTmp & cubeMask) == 0ull)
							break;

						if ((chunkLineTmp & airMask) != 0ull)
							break;

						if (this->at(x + tmpX, y + h, z) != type)
							break;

						tmpX++;
					}

					if (tmpX != w)
						break;

					for (int i = 0; i < w; i++)
					{
						chunkLeftBlock = ((chunkLeftLine2 >> (x + i)) & 1) & UINT32_MAX;
						chunkRightBlock = ((chunkRightLine2 >> (x + i)) & 1) & UINT32_MAX;

						chunkCurrLine = cBitmapL.axisZ[(x + i) + idY2] & UINT32_MAX;
						chunkLineTmp = (chunkRightBlock << 33) | (chunkCurrLine << 1) | chunkLeftBlock;

						length = trailing64One(chunkLineTmp >> zShift);
						cBitmapL.axisZ[(x + i) + idY2] -= (create64LengthMask(length) << (zShift - 1)) & UINT32_MAX;
					}
					h++;
				}

				// Create face
				pointLU = gm::Vec3f(x + w, y + h, z    );
				pointLD = gm::Vec3f(x + w, y    , z    );
				pointRD = gm::Vec3f(x    , y    , z    );
				pointRU = gm::Vec3f(x    , y + h, z    );
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
				chunkLeftLine = leftBitmap->axisZ[CHUNK_MAX + idY];
			else
				chunkLeftLine = 0ull;

			if (rightBitmap)
				chunkRightLine = rightBitmap->axisZ[0 + idY];
			else
				chunkRightLine = 0ull;

			// Face right
			chunkLeftBlock = ((chunkLeftLine >> z) & 1) & UINT32_MAX;
			chunkRightBlock = ((chunkRightLine >> z) & 1) & UINT32_MAX;
			chunkCurrLine = cBitmapR.axisX[z + idY] & UINT32_MAX;
			chunkLineR = (chunkLeftBlock << 33) | (chunkCurrLine << 1) | chunkRightBlock;
			while (chunkLineR != 0)
			{
				int x = trailing64Zero(chunkLineR);
				int	length = trailing64One(chunkLineR >> x);

				chunkLineR -= create64LengthMask(length) << x;

				if (x == 0 || x > CHUNK_SIZE)
					continue;

				// Create masks
				cubeMask = 1ull << x;
				airMask = 1ull << (x - 1);

				// Get real x and cube type
				int	xShift = x;
				x = CHUNK_MAX - (x - 1);
				const Cube	&type = this->at(x, y, z);

				// Extend in z axis
				int d = 1;
				while (z + d < CHUNK_SIZE)
				{
					chunkLeftBlock = ((chunkLeftLine >> (z + d)) & 1) & UINT32_MAX;
					chunkRightBlock = ((chunkRightLine >> (z + d)) & 1) & UINT32_MAX;
					chunkCurrLine = cBitmapR.axisX[(z + d) + idY] & UINT32_MAX;
					chunkLineTmp = (chunkLeftBlock << 33) | (chunkCurrLine << 1) | chunkRightBlock;

					if ((chunkLineTmp & cubeMask) == 0ull)
						break;

					if ((chunkLineTmp & airMask) != 0ull)
						break;

					if (this->at(x, y, z + d) != type)
						break;

					length = trailing64One(chunkLineTmp >> xShift);
					cBitmapR.axisX[(z + d) + idY] -= (create64LengthMask(length) << (xShift - 1)) & UINT32_MAX;
					d++;
				}

				// Extend in y axis
				int	h = 1;
				while (y + h < CHUNK_HEIGHT)
				{
					int	idY2 = (y + h) * CHUNK_SIZE;
					int	tmpZ = 0;
					uint32_t	chunkLeftLine2 = 0ull;
					uint32_t	chunkRightLine2 = 0ull;

					if (backBitmap)
						chunkLeftLine2 = backBitmap->axisZ[CHUNK_MAX + idY2];
					if (frontBitmap)
						chunkRightLine2 = frontBitmap->axisZ[0 + idY2];

					while (tmpZ < d)
					{
						chunkLeftBlock = ((chunkLeftLine2 >> (z + tmpZ)) & 1) & UINT32_MAX;
						chunkRightBlock = ((chunkRightLine2 >> (z + tmpZ)) & 1) & UINT32_MAX;
						chunkCurrLine = cBitmapR.axisX[(z + tmpZ) + idY2] & UINT32_MAX;
						chunkLineTmp = (chunkLeftBlock << 33) | (chunkCurrLine << 1) | chunkRightBlock;

						if ((chunkLineTmp & cubeMask) == 0ull)
							break;

						if ((chunkLineTmp & airMask) != 0ull)
							break;

						if (this->at(x, y + h, z + tmpZ) != type)
							break;

						tmpZ++;
					}

					if (tmpZ != d)
						break;

					for (int i = 0; i < d; i++)
					{
						chunkLeftBlock = ((chunkLeftLine2 >> (z + i)) & 1) & UINT32_MAX;
						chunkRightBlock = ((chunkRightLine2 >> (z + i)) & 1) & UINT32_MAX;

						chunkCurrLine = cBitmapR.axisX[(z + i) + idY2] & UINT32_MAX;
						chunkLineTmp = (chunkLeftBlock << 33) | (chunkCurrLine << 1) | chunkRightBlock;

						length = trailing64One(chunkLineTmp >> xShift);
						cBitmapR.axisX[(z + i) + idY2] -= (create64LengthMask(length) << (xShift - 1)) & UINT32_MAX;
					}
					h++;
				}

				// Create face
				pointLU = gm::Vec3f(x + 1, y + h, z + d);
				pointLD = gm::Vec3f(x + 1, y    , z + d);
				pointRD = gm::Vec3f(x + 1, y    , z    );
				pointRU = gm::Vec3f(x + 1, y + h, z    );
				createTriangleFace(vertexIndex, vertices, indices, nbVertex,
									pointLU, pointLD, pointRD, pointRU, normalRight, type);
			}

			// Face left
			chunkLeftBlock = ((chunkLeftLine >> z) & 1) & UINT32_MAX;
			chunkRightBlock = ((chunkRightLine >> z) & 1) & UINT32_MAX;
			chunkCurrLine = cBitmapL.axisX[z + idY] & UINT32_MAX;
			chunkLineL = (chunkRightBlock << 33) | (chunkCurrLine << 1) | chunkLeftBlock;
			while (chunkLineL != 0)
			{
				int x = trailing64Zero(chunkLineL);
				int	length = trailing64One(chunkLineL >> x);

				chunkLineL -= create64LengthMask(length) << x;

				if (x == 0 || x > CHUNK_SIZE)
					continue;

				// Create masks
				cubeMask = 1ull << x;
				airMask = 1ull << (x - 1);

				// Get real x and cube type
				int	xShift = x;
				x--;
				const Cube	&type = this->at(x, y, z);

				// Extend in z axis
				int d = 1;
				while (z + d < CHUNK_SIZE)
				{
					chunkLeftBlock = ((chunkLeftLine >> (z + d)) & 1) & UINT32_MAX;
					chunkRightBlock = ((chunkRightLine >> (z + d)) & 1) & UINT32_MAX;
					chunkCurrLine = cBitmapL.axisX[(z + d) + idY] & UINT32_MAX;
					chunkLineTmp = (chunkRightBlock << 33) | (chunkCurrLine << 1) | chunkLeftBlock;

					if ((chunkLineTmp & cubeMask) == 0ull)
						break;

					if ((chunkLineTmp & airMask) != 0ull)
						break;

					if (this->at(x, y, z + d) != type)
						break;

					length = trailing64One(chunkLineTmp >> xShift);
					cBitmapL.axisX[(z + d) + idY] -= (create64LengthMask(length) << (xShift - 1)) & UINT32_MAX;
					d++;
				}

				// Extend in y axis
				int	h = 1;
				while (y + h < CHUNK_HEIGHT)
				{
					int	idY2 = (y + h) * CHUNK_SIZE;
					int	tmpZ = 0;
					uint32_t	chunkLeftLine2 = 0ull;
					uint32_t	chunkRightLine2 = 0ull;

					if (backBitmap)
						chunkLeftLine2 = backBitmap->axisZ[CHUNK_MAX + idY2];
					if (frontBitmap)
						chunkRightLine2 = frontBitmap->axisZ[0 + idY2];

					while (tmpZ < d)
					{
						chunkLeftBlock = ((chunkLeftLine2 >> (z + tmpZ)) & 1) & UINT32_MAX;
						chunkRightBlock = ((chunkRightLine2 >> (z + tmpZ)) & 1) & UINT32_MAX;
						chunkCurrLine = cBitmapL.axisX[(z + tmpZ) + idY2] & UINT32_MAX;
						chunkLineTmp = (chunkRightBlock << 33) | (chunkCurrLine << 1) | chunkLeftBlock;

						if ((chunkLineTmp & cubeMask) == 0ull)
							break;

						if ((chunkLineTmp & airMask) != 0ull)
							break;

						if (this->at(x, y + h, z + tmpZ) != type)
							break;

						tmpZ++;
					}

					if (tmpZ != d)
						break;

					for (int i = 0; i < d; i++)
					{
						chunkLeftBlock = ((chunkLeftLine2 >> (z + i)) & 1) & UINT32_MAX;
						chunkRightBlock = ((chunkRightLine2 >> (z + i)) & 1) & UINT32_MAX;

						chunkCurrLine = cBitmapL.axisX[(z + i) + idY2] & UINT32_MAX;
						chunkLineTmp = (chunkRightBlock << 33) | (chunkCurrLine << 1) | chunkLeftBlock;

						length = trailing64One(chunkLineTmp >> xShift);
						cBitmapL.axisX[(z + i) + idY2] -= (create64LengthMask(length) << (xShift - 1)) & UINT32_MAX;
					}
					h++;
				}

				// Create face
				pointLU = gm::Vec3f(x    , y + h, z    );
				pointLD = gm::Vec3f(x    , y    , z    );
				pointRD = gm::Vec3f(x    , y    , z + d);
				pointRU = gm::Vec3f(x    , y + h, z + d);
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
