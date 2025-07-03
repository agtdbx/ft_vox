#include <program/map/Chunk.hpp>

#include <program/map/Map.hpp>

#include <unordered_map>

PerlinNoise	createNoise(const gm::Vec2i &shape, unsigned int octaves, float persistence)
{
	static bool	randomInit = false;

	if (!randomInit)
	{
		randomInit = true;
		gm::initRandom();
	}

	return PerlinNoise(gm::uRand(), shape, octaves, persistence);
}

PerlinNoise	perlinTerrainLevel = createNoise(gm::Vec2i(128, 128), 1, 0.0f);
PerlinNoise	perlinTerrainModifer = createNoise(gm::Vec2i(128, 128), 4, 0.3);
PerlinNoise	perlinBiome = createNoise(gm::Vec2i(128, 128), 1, 0.0f);
PerlinNoise	perlinCaveSize = createNoise(gm::Vec2i(128, 128), 1, 0.0f);
PerlinNoise	perlinCaveHeight = createNoise(gm::Vec2i(128, 128), 1, 0.0f);
PerlinNoise	perlinMineral = createNoise(gm::Vec2i(128, 128), 1, 0.0f);

const gm::Vec3f	CHUNK_MIDDLE_OFFSET(CHUNK_SIZE / 2, CHUNK_HEIGHT / 2, CHUNK_SIZE / 2);
const float	scaleTerrainLevel = 1.0f / 256.0f;
const float	scaleTerrainModifier = 1.0f / 128.0f;
const float	scaleBiome = 1.0f / 1024.0f;
const float	scaleCaveSize = 1.0f / 64.0f;
const float	scaleCaveHeightX = 1.0f / 256.0f;
const float	scaleCaveHeightY = 1.0f / 64.0f;
const float	scaleMineral = 1.0f / 10.0f;

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** PUBLIC METHODS **********************************************************

void	Chunk::generate(const gm::Vec2i &chunkId, PerfLogger &perfLogger)
{
	if (this->generationDone)
		return ;
	this->generationDone = true;

	perflogStart(perfLogger.generateChunk);

	this->chunkId = chunkId;
	this->chunkPosition.x = this->chunkId.x * CHUNK_SIZE;
	this->chunkPosition.y = 0.0f;
	this->chunkPosition.z = this->chunkId.y * CHUNK_SIZE;

	this->boundingCube.center = this->chunkPosition + CHUNK_MIDDLE_OFFSET;
	this->boundingCube.computePoints();

	int		idZ, idXZ, id, height, maxHeight;
	float	perlinX, perlinZ,
			baseHeight, modifierHeight, biome,
			caveSize, caveHeight, mineral, diffCave;

	for (int z = 0; z < CHUNK_SIZE; z++)
	{
		idZ = z * CHUNK_SIZE;
		perlinZ = this->chunkPosition.z + z;
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			idXZ = x + idZ;
			perlinX = this->chunkPosition.x + x;

			baseHeight = perlinTerrainLevel.getNoise(perlinX * scaleTerrainLevel, perlinZ * scaleTerrainLevel);
			baseHeight = (baseHeight * baseHeight * baseHeight) * 100.0f + baseHeight * 50 + 80.0f;

			modifierHeight = perlinTerrainModifer.getNoise(perlinX * scaleTerrainModifier, perlinZ * scaleTerrainModifier);
			modifierHeight = modifierHeight * 16.0f;

			biome = perlinBiome.getNoise(perlinX * scaleBiome, perlinZ * scaleBiome);

			height = baseHeight + modifierHeight;
			maxHeight = gm::max(height, CHUNK_WATER_LEVEL);

			caveSize = perlinCaveSize.getNoiseNormalize(perlinX * scaleCaveSize, perlinZ * scaleCaveSize);
			caveSize = gm::max(caveSize - 0.7f, 0.0f) * 42.0f;

			if (caveSize <= 0.0f)
			{
				caveHeight = 0.0f;
				mineral = 0.0f;
			}
			else
			{
				caveHeight = perlinCaveHeight.getNoiseNormalize(perlinX * scaleCaveHeightX, perlinZ * scaleCaveHeightY);
				caveHeight = caveHeight * 42.0f + 12.0;

				mineral = perlinMineral.getNoise(perlinX * scaleMineral, perlinZ * scaleMineral);
			}

			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				id = idXZ + y * CHUNK_SIZE2;

				if (y == 0)
				{
					this->cubes[id] = CUBE_STONE;
					this->cubeBitmap.set(x, y, z, true);
					continue;
				}

				if (y > maxHeight)
					break;

				if (y <= height)
				{
					diffCave = gm::abs(y - caveHeight);
					if (diffCave < caveSize)
						continue;
					else if (diffCave < caveSize + 1)
					{
						if (mineral >= 0.5f)
							this->cubes[id] = CUBE_IRON;
						else if (mineral <= -0.75f)
							this->cubes[id] = CUBE_DIAMOND;
						else
							this->cubes[id] = CUBE_STONE;
						this->cubeBitmap.set(x, y, z, true);
						continue;
					}
					else if (y < height - 4)
					{
						this->cubes[id] = CUBE_STONE;
						this->cubeBitmap.set(x, y, z, true);
						continue;
					}

					// Cold biome
					if (biome < -0.4)
					{
						if (y <= height && y >= height - 4)
						{
							this->cubes[id] = CUBE_SNOW;
							this->cubeBitmap.set(x, y, z, true);
						}
					}
					// Hot biome
					else if (biome > 0.4)
					{
						if (y <= height && y >= height - 4)
						{
							this->cubes[id] = CUBE_SAND;
							this->cubeBitmap.set(x, y, z, true);
						}
					}
					// Normal biome
					else
					{
						if (y == height && y >= CHUNK_WATER_LEVEL)
						{
							this->cubes[id] = CUBE_GRASS;
							this->cubeBitmap.set(x, y, z, true);
						}
						else if (y >= height - 4)
						{
							this->cubes[id] = CUBE_DIRT;
							this->cubeBitmap.set(x, y, z, true);
						}
					}
				}
				else
				{
					// Cold biome
					if (biome < -0.4)
					{
						if (y >= CHUNK_WATER_LEVEL)
						{
							this->cubes[id] = CUBE_ICE;
							this->cubeBitmap.set(x, y, z, true);
						}
					}
					// Hot biome
					else if (biome > 0.4)
					{
						if (y >= CHUNK_WATER_LEVEL)
						{
							this->cubes[id] = CUBE_LAVA;
							this->cubeBitmap.set(x, y, z, true);
						}
					}
					// Normal biome
					else
					{
						if (y >= CHUNK_WATER_LEVEL)
							this->cubes[id] = CUBE_WATER;
					}

				}
			}
		}
	}

	perflogEnd(perfLogger.generateChunk);
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
