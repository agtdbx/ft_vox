#include <program/map/Chunk.hpp>

#include <program/map/Map.hpp>

#include <unordered_map>

// TODO : Random seed ?
PerlinNoise	perlinTerrainLevel(42, gm::Vec2i(128, 128));
PerlinNoise	perlinTerrainModifer(69, gm::Vec2i(128, 128), 4, 0.3);
PerlinNoise	perlinBiome(7, gm::Vec2i(128, 128));
PerlinNoise	perlinCaveSize(23, gm::Vec2i(128, 128));
PerlinNoise	perlinCaveHeight(90, gm::Vec2i(128, 128));
PerlinNoise	perlinMineral(32, gm::Vec2i(128, 128));

const gm::Vec3f	CHUNK_MIDDLE_OFFSET(CHUNK_SIZE / 2, CHUNK_HEIGHT / 2, CHUNK_SIZE / 2);

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

	for (int z = 0; z < CHUNK_SIZE; z++)
	{
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			float	perlinX = this->chunkPosition.x + x;
			float	perlinZ = this->chunkPosition.z + z;

			float	baseHeight = perlinTerrainLevel.getNoise(perlinX / 1024.0f, perlinZ / 1024.0f);
			baseHeight = (baseHeight + 0.4) * 100.0f + 64.0f;

			float	modifierHeight = perlinTerrainModifer.getNoise(perlinX / 128.0f, perlinZ / 128.0f);
			modifierHeight = modifierHeight * 16.0f;

			float	biome = perlinBiome.getNoise(perlinX / 1024.0f, perlinZ / 1024.0f);

			int		height = baseHeight + modifierHeight;
			int		maxHeight = gm::max(height, CHUNK_WATER_LEVEL);

			float	caveSize = perlinCaveSize.getNoiseNormalize(perlinX / 64.0f, perlinZ / 64.0f);
			caveSize = gm::max(caveSize - 0.7f, 0.0f) * 42.0f;

			float	caveHeight = perlinCaveHeight.getNoiseNormalize(perlinX / 256.0f, perlinZ / 64.0f);
			caveHeight = caveHeight * 42.0f + 12.0;

			float	mineral = perlinMineral.getNoise(perlinX / 10.0f, perlinZ / 10.0f);

			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				if (y == 0)
				{
					this->setCube(x, y, z, CUBE_STONE);
					continue;
				}

				if (y > maxHeight)
					break;

				if (y <= height)
				{
					float	diffCave = gm::abs(y - caveHeight);
					if (diffCave < caveSize)
						continue;
					else if (diffCave < caveSize + 1)
					{
						if (mineral >= 0.5f)
							this->setCube(x, y, z, CUBE_IRON);
						else if (mineral <= -0.75f)
							this->setCube(x, y, z, CUBE_DIAMOND);
						else
							this->setCube(x, y, z, CUBE_STONE);
						continue;
					}
					else if (y < height - 4)
					{
						this->setCube(x, y, z, CUBE_STONE);
						continue;
					}

					// Cold biome
					if (biome < -0.3)
					{
						if (y <= height && y >= height - 4)
							this->setCube(x, y, z, CUBE_SNOW);
					}
					// Hot biome
					else if (biome > 0.3)
					{
						if (y <= height && y >= height - 4)
							this->setCube(x, y, z, CUBE_SAND);
					}
					// Normal biome
					else
					{
						if (y == height && y >= CHUNK_WATER_LEVEL)
							this->setCube(x, y, z, CUBE_GRASS);
						else if (y >= height - 4)
							this->setCube(x, y, z, CUBE_DIRT);
					}
				}
				else
				{
					// Cold biome
					if (biome < -0.3)
					{
						if (y >= CHUNK_WATER_LEVEL)
							this->setCube(x, y, z, CUBE_ICE);
					}
					// Hot biome
					else if (biome > 0.3)
					{
						if (y >= CHUNK_WATER_LEVEL)
							this->setCube(x, y, z, CUBE_LAVA);
					}
					// Normal biome
					else
					{
						if (y >= CHUNK_WATER_LEVEL)
							this->setCube(x, y, z, CUBE_WATER);
					}

				}
			}
		}
	}

	perflogEnd(perfLogger.generateChunk);
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
