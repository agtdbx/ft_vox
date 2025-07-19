#include <program/map/Chunk.hpp>

#include <program/map/Map.hpp>

#include <unordered_map>

//**** STATIC FUNCTIONS DEFINE *************************************************

static PerlinNoise	createNoise(const gm::Vec2i &shape, unsigned int octaves, float persistence);

//**** STATIC VARIABLES DEFINE *************************************************

const PerlinNoise	perlinSea = createNoise(gm::Vec2i(128, 128), 4, 0.3);
const PerlinNoise	perlinPlaine = createNoise(gm::Vec2i(128, 128), 4, 0.3f);
const PerlinNoise	perlinMountain = createNoise(gm::Vec2i(128, 128), 4, 0.5f);
const PerlinNoise	perlinBiomeHeight = createNoise(gm::Vec2i(128, 128), 1, 0.0f);
const PerlinNoise	perlinBiomeTemp = createNoise(gm::Vec2i(128, 128), 1, 0.0f);
const PerlinNoise	perlinCaveSize1 = createNoise(gm::Vec2i(128, 128), 1, 0.0f);
const PerlinNoise	perlinCaveSize2 = createNoise(gm::Vec2i(128, 128), 1, 0.0f);
const PerlinNoise	perlinCaveHeight = createNoise(gm::Vec2i(128, 128), 1, 0.0f);
const PerlinNoise	perlinMineral = createNoise(gm::Vec2i(128, 128), 1, 0.0f);

const gm::Vec3f	CHUNK_MIDDLE_OFFSET(CHUNK_SIZE / 2, CHUNK_HEIGHT / 2, CHUNK_SIZE / 2);
const float	scaleSea = 1.0f / 256.0f;
const float	scalePlaine = 1.0f / 128.0f;
const float	scaleToundra = 1.0f / 192.0f;
const float	scaleDesert = 1.0f / 64.0f;
const float	scaleMountain = 1.0f / 128.0f;
const float	scaleVolcano = 1.0f / 192.0f;
const float	scaleMessa = 1.0f / 256.0f;
const float	scaleBiomeHeight = 1.0f / 1024.0f;
const float	scaleBiomeTemp = 1.0f / 1024.0f;
const float	scaleCaveSize1 = 1.0f / 64.0f;
const float	scaleCaveSize2 = 1.0f / 128.0f;
const float	scaleCaveHeightX = 1.0f / 256.0f;
const float	scaleCaveHeightY = 1.0f / 64.0f;
const float	scaleMineral = 1.0f / 10.0f;

//**** PUBLIC METHODS **********************************************************

void	Chunk::generate(const gm::Vec2i &chunkId)
{
	if (this->generationDone)
		return ;
	this->generationDone = true;

	this->chunkId = chunkId;
	this->chunkPosition.x = this->chunkId.x * CHUNK_SIZE;
	this->chunkPosition.y = 0.0f;
	this->chunkPosition.z = this->chunkId.y * CHUNK_SIZE;

	this->boundingCube.center = this->chunkPosition + CHUNK_MIDDLE_OFFSET;
	this->boundingCube.computePoints();

	int		idZ, idXZ, id, height, maxHeight;
	float	perlinX, perlinZ,
			seaHeight, plaineHeight, mountainHeight, baseHeight, biomeHeight, biomeTemp,
			caveSize, caveSize1, caveSize2, caveHeight, mineral, diffCave;
	bool	seaBiome, mountainBiome, coldBiome, hotBiome, lavaHotEnough;

	for (int z = 0; z < CHUNK_SIZE; z++)
	{
		idZ = z * CHUNK_SIZE;
		perlinZ = this->chunkPosition.z + z;
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			idXZ = x + idZ;
			perlinX = this->chunkPosition.x + x;

			// biomeTemp = perlinBiomeTemp.getNoise(perlinX * scaleBiomeTemp, perlinZ * scaleBiomeTemp);
			biomeTemp = -0.7f;
			coldBiome = (biomeTemp < -0.4f);
			hotBiome = (biomeTemp > 0.4f);
			lavaHotEnough = (biomeTemp > 0.5f);

			// Cold version
			if (coldBiome)
			{
				biomeHeight = perlinBiomeHeight.getNoise(perlinX * scaleBiomeHeight, perlinZ * scaleBiomeHeight);
				plaineHeight = perlinPlaine.getNoise(perlinX * scaleToundra, perlinZ * scaleToundra) * 20.0f + 70.0f;
				if (biomeHeight < -0.2f)
				{
					seaHeight = perlinSea.getNoise(perlinX * scaleSea, perlinZ * scaleSea) * 10.0f - 10.0f;
					baseHeight = gm::lerp(plaineHeight, seaHeight, -(biomeHeight + 0.2f));
					seaBiome = true;
					mountainBiome = false;
				}
				else
				{
					mountainHeight = perlinMountain.getNoise(perlinX * scaleVolcano, perlinZ * scaleVolcano) * 400.0f + 200.0f;
					baseHeight = gm::lerp(plaineHeight, mountainHeight, gm::max(biomeHeight - 0.5f, 0.0f) / 0.5f);
					seaBiome = false;
					mountainBiome = (biomeHeight - 0.5f > 0.0f);
				}
			}
			// Hot version
			else if (hotBiome)
			{
				biomeHeight = perlinBiomeHeight.getNoise(perlinX * scaleBiomeHeight, perlinZ * scaleBiomeHeight);
				plaineHeight = perlinPlaine.getNoise(perlinX * scaleDesert, perlinZ * scaleDesert) * 30.0f + 80.0f;
				if (biomeHeight < 0.0f)
				{
					seaHeight = perlinSea.getNoise(perlinX * scaleSea, perlinZ * scaleSea) * 10.0f - 10.0f;
					baseHeight = gm::lerp(plaineHeight, seaHeight, -biomeHeight);
					seaBiome = true;
					mountainBiome = false;
				}
				else
				{
					mountainHeight = perlinMountain.getNoise(perlinX * scaleMessa, perlinZ * scaleMessa);
					baseHeight = gm::lerp(plaineHeight, mountainHeight, biomeHeight);
					if (mountainHeight > 0.0f && (int)(mountainHeight * 10.0f) % 2)
					{
						baseHeight = mountainHeight * 30.0f + plaineHeight / 10.0f + 80.0f;
						mountainBiome = true;
					}
					else
					{
						baseHeight = plaineHeight;
						mountainBiome = false;
					}
					seaBiome = false;
				}
			}
			// Classic version
			else
			{
				biomeHeight = perlinBiomeHeight.getNoise(perlinX * scaleBiomeHeight, perlinZ * scaleBiomeHeight);
				plaineHeight = perlinPlaine.getNoise(perlinX * scalePlaine, perlinZ * scalePlaine) * 20.0f + 70.0f;
				if (biomeHeight < 0.0f)
				{
					seaHeight = perlinSea.getNoise(perlinX * scaleSea, perlinZ * scaleSea) * 10.0f - 10.0f;
					baseHeight = gm::lerp(plaineHeight, seaHeight, -biomeHeight);
					seaBiome = true;
					mountainBiome = false;
				}
				else
				{
					mountainHeight = perlinMountain.getNoise(perlinX * scaleMountain, perlinZ * scaleMountain) * 400.0f + 140.0f;
					baseHeight = gm::lerp(plaineHeight, mountainHeight, gm::max(biomeHeight - 0.4f, 0.0f) / 0.6f);
					seaBiome = false;
					mountainBiome = (biomeHeight - 0.4f > 0.0f);
				}
			}

			// Terrain height
			height = baseHeight;
			maxHeight = gm::max(height, CHUNK_LIQUID_LEVEL);

			// Cave
			caveSize1 = perlinCaveSize1.getNoiseNormalize(perlinX * scaleCaveSize1, perlinZ * scaleCaveSize2);
			caveSize2 = perlinCaveSize2.getNoiseNormalize(perlinX * scaleCaveSize2, perlinZ * scaleCaveSize1);
			caveSize = gm::max(gm::max(caveSize1, caveSize2) - 0.6f, 0.0f) * 42.0f;

			if (caveSize <= 0.0f)
			{
				caveHeight = 0.0f;
				mineral = 0.0f;
			}
			else
			{
				caveHeight = perlinCaveHeight.getNoiseNormalize(perlinX * scaleCaveHeightX, perlinZ * scaleCaveHeightY);
				caveHeight = caveHeight * baseHeight;

				mineral = perlinMineral.getNoise(perlinX * scaleMineral, perlinZ * scaleMineral);
			}

			// Put blocs
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

				// Cave
				diffCave = gm::abs(y - caveHeight);
				if (diffCave < caveSize)
					continue;
				else if (diffCave < caveSize + 1)
				{
					if (mineral >= 0.7f)
						this->cubes[id] = CUBE_IRON;
					else if (mineral <= -0.9f)
						this->cubes[id] = CUBE_DIAMOND;
					else
						this->cubes[id] = CUBE_STONE;
					this->cubeBitmap.set(x, y, z, true);
					continue;
				}

				// Put stone for most terrain body
				if (y < height - 4)
				{
					this->cubes[id] = CUBE_STONE;
					this->cubeBitmap.set(x, y, z, true);
					continue;
				}

				// Cold version
				if (coldBiome)
				{
					if (y <= height)
					{
						// Mountain top
						if (mountainBiome && y > 175)
						{
							this->cubes[id] = CUBE_SNOW;
							this->cubeBitmap.set(x, y, z, true);
						}
						// Mountain
						else if (mountainBiome && y > 75)
						{
							this->cubes[id] = CUBE_STONE;
							this->cubeBitmap.set(x, y, z, true);
						}
						// Plaine
						else
						{
							this->cubes[id] = CUBE_SNOW;
							this->cubeBitmap.set(x, y, z, true);
						}
					}
					else if (y >= CHUNK_LIQUID_LEVEL)
					{
						this->cubes[id] = CUBE_ICE;
						this->cubeBitmap.set(x, y, z, true);
					}
					else
					{
						this->cubes[id] = CUBE_WATER;
					}
				}
				// Hot version
				else if (hotBiome)
				{
					if (y <= height)
					{
						// Sand beach
						if (seaBiome && y < CHUNK_LIQUID_LEVEL + 3)
						{
							this->cubes[id] = CUBE_SAND;
							this->cubeBitmap.set(x, y, z, true);
						}
						// Mountain top
						else if (mountainBiome && y > 175)
						{
							this->cubes[id] = CUBE_SNOW;
							this->cubeBitmap.set(x, y, z, true);
						}
						// Mountain
						else if (mountainBiome && y > 75)
						{
							this->cubes[id] = CUBE_STONE;
							this->cubeBitmap.set(x, y, z, true);
						}
						// Plaine
						else
						{
							this->cubes[id] = CUBE_SAND;
							this->cubeBitmap.set(x, y, z, true);
						}
					}
					else
					{
						if (lavaHotEnough)
							this->cubes[id] = CUBE_LAVA;
						else
							this->cubes[id] = CUBE_WATER;
					}
				}
				// Classic version
				else
				{
					if (y <= height)
					{
						// Sand beach
						if (seaBiome && y < CHUNK_LIQUID_LEVEL + 3)
						{
							this->cubes[id] = CUBE_SAND;
							this->cubeBitmap.set(x, y, z, true);
						}
						// Mountain top
						else if (mountainBiome && y > 175)
						{
							this->cubes[id] = CUBE_SNOW;
							this->cubeBitmap.set(x, y, z, true);
						}
						// Mountain
						else if (mountainBiome && y > 75)
						{
							this->cubes[id] = CUBE_STONE;
							this->cubeBitmap.set(x, y, z, true);
						}
						// Plaine
						else
						{
							if (y == height && y >= CHUNK_LIQUID_LEVEL)
							{
								this->cubes[id] = CUBE_GRASS;
								this->cubeBitmap.set(x, y, z, true);
							}
							else
							{
								this->cubes[id] = CUBE_DIRT;
								this->cubeBitmap.set(x, y, z, true);
							}
						}
					}
					else
					{
						this->cubes[id] = CUBE_WATER;
					}
				}
			}
		}
	}
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static PerlinNoise	createNoise(const gm::Vec2i &shape, unsigned int octaves, float persistence)
{
	static bool	randomInit = false;

	if (!randomInit)
	{
		randomInit = true;
		#ifndef SEED
		uint seed = gm::initRandom();
		#else
		uint seed = SEED;
		gm::initRandom(seed);
		#endif
		std::cout << "SEED : " << seed << std::endl;
	}

	return PerlinNoise(gm::uRand(), shape, octaves, persistence);
}
