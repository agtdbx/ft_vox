#ifndef PERLINNOISE_HPP
# define PERLINNOISE_HPP

# include <gmath.hpp>

#define SEED 42
#define SEEDTERRAIN SEED * 2
#define SEEDBIOME SEED * 5
#define PERLIN_NOISE_SIZE 42
const int PERLIN_NOISE_LIST_SIZE = PERLIN_NOISE_SIZE * 2;
const int PERLIN_NOISE_MASK = PERLIN_NOISE_LIST_SIZE - 1;

#define GENERATION 1
#define TERRAIN 2
#define BIOME 3


/**
 * @brief PerlinNoise class.
 */
class PerlinNoise
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of PerlinNoise class.
	 *
	 * @return The default PerlinNoise.
	 */
	PerlinNoise(void);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of PerlinNoise class.
	 */
	~PerlinNoise();


//**** PUBLIC METHODS **********************************************************

    float goToNoise(float x, float y, int type);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************

    //TODO Avoir 1 seul Perlin Noise est donc 1 seul seed
    int	perlin_noiseGeneration[PERLIN_NOISE_LIST_SIZE]  = {0};
    int	perlin_noiseTerrain[PERLIN_NOISE_LIST_SIZE]  = {0};
    int	perlin_noiseBiome[PERLIN_NOISE_LIST_SIZE]  = {0};
    int         seed = SEED;
    int         seedBiome = SEEDBIOME;
    int         seedTerrain = SEEDTERRAIN;

//**** PRIVATE METHODS *********************************************************

    gm::Vec2f	get_constant_vector(int value);
    //TODO passez cette fonction ne publique
    float getNoise(float x, float y, int perlin_noisef[PERLIN_NOISE_LIST_SIZE]);
};

//**** FUNCTIONS ***************************************************************

#endif
