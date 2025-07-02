#ifndef PERLIN_NOISE_HPP
# define PERLIN_NOISE_HPP

# include <gmath.hpp>

# include <vector>

# define DEFAULT_PERLIN_SHAPE gm::Vec2i(16, 16)

struct	Noise
{
	gm::Vec2i				shape;
	std::vector<gm::Vec2f>	noise;

	Noise	&operator=(const Noise &obj)
	{
		if (this == &obj)
			return (*this);

		this->shape = obj.shape;
		this->noise = obj.noise;

		return (*this);
	}
};

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
	 * @brief Copy constructor of PerlinNoise class.
	 *
	 * @param obj The PerlinNoise to copy.
	 *
	 * @return The PerlinNoise copied from parameter.
	 */
	PerlinNoise(const PerlinNoise &obj);
	/**
	 * @brief Constructor of PerlinNoise class.
	 *
	 * @param seed The seed of perlin noise.
	 * @param shape The shape of perlin noise.
	 *
	 * @return The PerlinNoise created from parameter.
	 */
	PerlinNoise(unsigned int seed, const gm::Vec2i &shape);
	/**
	 * @brief Constructor of PerlinNoise class.
	 *
	 * @param seed The seed of perlin noise.
	 * @param shape The shape of perlin noise.
	 * @param octaves The number of sub perlin noise (for more details). Must be higher than 0.
	 * @param persistence The influence of sub perlin noise on previous noise. Must be between 0 and 1.
	 *
	 * @exception Throw an runtime_error if octaves is 0 or if persistence isn't in range [0, 1].
	 * @return The PerlinNoise created from parameter.
	 */
	PerlinNoise(
		unsigned int seed,
		const gm::Vec2i &shape,
		unsigned int octaves,
		float persistence);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of PerlinNoise class.
	 */
	~PerlinNoise();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of PerlinNoise class.
	 *
	 * @param obj The PerlinNoise to copy.
	 *
	 * @return The PerlinNoise copied from parameter.
	 */
	PerlinNoise	&operator=(const PerlinNoise &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Method to get noise at give coordonates.
	 *
	 * @param x The x coordonates.
	 * @param y The y coordonates.
	 *
	 * @return The noise a coordonates (x, y). Result is between -1 and 1.
	 */
	float	getNoise(float x, float y) const;
	/**
	 * @brief Method to get noise normalize at give coordonates.
	 *
	 * @param x The x coordonates.
	 * @param y The y coordonates.
	 *
	 * @return The normalize noise a coordonates (x, y). Result is between 0 and 1.
	 */
	float	getNoiseNormalize(float x, float y) const;

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	unsigned int		seed, octaves;
	float				persistence;
	std::vector<Noise>	noises;

//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Generate noises.
	 *
	 * @param firstShape Shape of the first noise.
	 */
	void	generateNoises(const gm::Vec2i &firstShape);
};

//**** FUNCTIONS ***************************************************************

#endif
