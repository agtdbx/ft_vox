#ifndef PERLINNOISE_HPP
# define PERLINNOISE_HPP

# include <gmath.hpp>
# include <vector>

#define SEED 42
#define PERLIN_NOISE_SIZE 64 //must be a power of 2

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
	 * @brief contructor of PerlinNoise class.
	 *
	 * @param seed The seed of the perlin.
	 *
	 * @param perlin_noise_size The size of the perlin noise.
	 *
	 * @return The default PerlinNoise.
	 */
	PerlinNoise(int seed, int perlin_noise_size);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of PerlinNoise class.
	 */
	~PerlinNoise();


//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Get the noise between 0 and 1 at a given coordinate
	 *
	 * @param x The x coordinate.
	 *
	 * @param y The y coordinate.
	 *
	 * @return Return the noise between 0 and 1 at a given coordinate.
	 */
    float getNoise(float x, float y);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************

    std::vector<int>	perlin_noise;
    int         		seed;
	int 				perlin_noise_size;
	int 				perlin_noise_list_size;
	int 				perlin_noise_mask;

//**** PRIVATE METHODS *********************************************************

	/**
	 * @brief Compare the direction of vector for the noise
	 *
	 * @param value Use to find the direction of the vector.
	 *
	 * @return Return the vector with the correct axis.
	 */
    gm::Vec2f	get_constant_vector(int value);
};

//**** FUNCTIONS ***************************************************************

#endif
