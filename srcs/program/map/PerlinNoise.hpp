#ifndef PERLINNOISE_HPP
# define PERLINNOISE_HPP

# include <gmath.hpp>

#define SEED 42
#define SEEDTERRAIN SEED * 2
#define SEEDBIOME SEED * 5
#define PERLIN_NOISE_SIZE 42
#define PERLIN_NOISE_LIST_SIZE PERLIN_NOISE_SIZE * 2
#define PERLIN_NOISE_MASK PERLIN_NOISE_LIST_SIZE - 1

#define GENERATION 1
#define TERRAIN 2
#define BIOME 3


/**
 * @brief Chunk class.
 */
class PerlinNoise
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Chunk class.
	 *
	 * @return The default Chunk.
	 */
	PerlinNoise(void);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Chunk class.
	 */
	~PerlinNoise();


//**** PUBLIC METHODS **********************************************************

    float goToNoise(float x, float y, int type);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************

    int	perlin_noiseGeneration[PERLIN_NOISE_LIST_SIZE]  = {0};
    int	perlin_noiseTerrain[PERLIN_NOISE_LIST_SIZE]  = {0};
    int	perlin_noiseBiome[PERLIN_NOISE_LIST_SIZE]  = {0};
    int         seed = SEED;
    int         seedBiome = SEEDBIOME;
    int         seedTerrain = SEEDTERRAIN;


//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
