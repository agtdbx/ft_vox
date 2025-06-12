#ifndef PERLINNOISE_HPP
# define PERLINNOISE_HPP

# include <gmath.hpp>

#define SEED 42
#define PERLIN_NOISE_SIZE 64
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

    /**
	 * @brief Default contructor of PerlinNoise class.
	 *
	 * @return The default PerlinNoise.
	 */
	PerlinNoise(int seed);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of PerlinNoise class.
	 */
	~PerlinNoise();


//**** PUBLIC METHODS **********************************************************
    float getNoise(float x, float y);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************

    int	perlin_noise[PERLIN_NOISE_LIST_SIZE]  = {0};
    int         seed;

//**** PRIVATE METHODS *********************************************************

    gm::Vec2f	get_constant_vector(int value);
};

//**** FUNCTIONS ***************************************************************

#endif
