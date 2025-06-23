#include <program/map/Chunk.hpp>

#include <program/map/Map.hpp>

#include <unordered_map>

PerlinNoise PerlinGeration(42, 64);
PerlinNoise PerlinTerrain(854, 2);
PerlinNoise PerlinBiome(654, 4096);
//PerlinNoise PerlinCave(8576, 128);

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** PUBLIC METHODS **********************************************************
void	Chunk::generate(const gm::Vec2i &chunkId, PerfLogger &perfLogger)
{
	perflogStart(perfLogger.generateChunk);

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
	int		idZ, id;
	//float Cave = 0;
	for (int z = 0; z < CHUNK_SIZE; z++)
	{
		idZ = z * CHUNK_SIZE;
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
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
				// //with this setup stone cannot be seen on the surface
				if (y > (int)maxSize && y > CHUNK_WATER_LEVEL)
					break;
				id = x + idZ + y * CHUNK_SIZE2;

				if (y > (int)maxSize && y <= CHUNK_WATER_LEVEL)
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
					this->cubeBitmap.set(x, y, z, true);
				}
				// End
				else
				{
					this->cubeBitmap.set(x, y, z, true);
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

	perflogEnd(perfLogger.generateChunk);

	// TODO : REMOVE
	if (this->chunkId == gm::Vec2i(0, 0))
	{
		this->setCube(10, 200, 10, CUBE_STONE);
		this->setCube(11, 200, 10, CUBE_STONE);
		this->setCube(11, 200, 11, CUBE_STONE);
		this->setCube(10, 200, 11, CUBE_STONE);
		this->setCube(10, 201, 10, CUBE_STONE);
		this->setCube(11, 201, 10, CUBE_STONE);
		this->setCube(11, 201, 11, CUBE_STONE);
		this->setCube(10, 201, 11, CUBE_STONE);
		this->setCube(11, 200, 12, CUBE_SAND);
		this->setCube(10, 200, 12, CUBE_SAND);
		this->setCube(11, 201, 12, CUBE_SAND);
		this->setCube(10, 201, 12, CUBE_SAND);
		this->setCube(12, 200, 10, CUBE_SAND);
		this->setCube(12, 201, 10, CUBE_SAND);
		this->setCube(12, 200, 12, CUBE_STONE);
		this->setCube(12, 201, 12, CUBE_STONE);
	}
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
