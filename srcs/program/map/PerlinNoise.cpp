#include <program/map/PerlinNoise.hpp>

#include <stdexcept>

//**** STATIC FUNCTIONS DEFINE *************************************************

static void		generateNoise(Noise &noise, unsigned int seed);
static float	getNoiseValue(const Noise &noise, float x, float y);
static float	fade(float value);

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

PerlinNoise::PerlinNoise(void)
{
	this->seed = 42;
	this->octaves = 1;
	this->persistence = 0.0f;
	this->generateNoises(DEFAULT_PERLIN_SHAPE);
}


PerlinNoise::PerlinNoise(const PerlinNoise &obj)
{
	this->seed = obj.seed;
	this->octaves = obj.octaves;
	this->persistence = obj.persistence;
	this->noises = obj.noises;
}


PerlinNoise::PerlinNoise(unsigned int seed, const gm::Vec2i &shape)
{
	this->seed = seed;
	this->octaves = 1;
	this->persistence = 0.0f;
	this->generateNoises(shape);
}


PerlinNoise::PerlinNoise(
				unsigned int seed,
				const gm::Vec2i &shape,
				unsigned int octaves,
				float persistence)
{
	this->seed = seed;
	this->octaves = octaves;
	this->persistence = persistence;

	if (this->octaves == 0)
		throw std::runtime_error("Octave can't  be 0");
	if (persistence < 0.0f || persistence > 1.0f)
		throw std::runtime_error("Persistence must be between 0.0 and 1.0");

	this->generateNoises(shape);
}

//---- Destructor --------------------------------------------------------------

PerlinNoise::~PerlinNoise()
{
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

PerlinNoise	&PerlinNoise::operator=(const PerlinNoise &obj)
{
	if (this == &obj)
		return (*this);

	this->seed = obj.seed;
	this->octaves = obj.octaves;
	this->persistence = obj.persistence;
	this->noises = obj.noises;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

float	PerlinNoise::getNoise(float x, float y) const
{
	if (this->octaves == 1 || this->persistence == 0.0f)
		return (getNoiseValue(this->noises[0], x, y));

	float	total = 0.0f;
	float	frequency =  1.0f;
	float	amplitude = 1.0f;
	float	maxValue = 0.0f;

	for (unsigned int i = 0; i < this->octaves; i++)
	{
		total += getNoiseValue(this->noises[i], x * frequency, y * frequency) * amplitude;

		maxValue += amplitude;

		amplitude *= persistence;
		if (amplitude <= 0.00001f)
			break;
		frequency *= 2.0f;
	}

	return (total / maxValue);
}


float	PerlinNoise::getNoiseNormalize(float x, float y) const
{
	float	noise = this->getNoise(x, y);

	return ((noise + 1.0f) * 0.5f);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	PerlinNoise::generateNoises(const gm::Vec2i &firstShape)
{
	unsigned int	seed = this->seed;
	gm::Vec2i		shape = firstShape;

	this->noises.resize(this->octaves);
	for (unsigned int i = 0; i < this->octaves; i++)
	{
		// Create noise
		this->noises[i].shape = shape;
		generateNoise(this->noises[i], seed);

		// Create seed and shape for next noise
		if (i + 1 < this->octaves)
		{
			gm::initRandom(seed);
			seed = gm::uRand();
			shape *= 2;
		}
	}
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static void	generateNoise(Noise &noise, unsigned int seed)
{
	int size = noise.shape.x * noise.shape.y;

	gm::initRandom(seed);

	noise.noise.resize(size);
	for (int i = 0; i < size; i++)
	{
		int	nb = gm::iRand(0, 3);

		if (nb == 0)
			noise.noise[i] = gm::Vec2f( 1.0f, 1.0f);
		else if (nb == 1)
			noise.noise[i] = gm::Vec2f(-1.0f, 1.0f);
		else if (nb == 2)
			noise.noise[i] = gm::Vec2f(-1.0f,-1.0f);
		else
			noise.noise[i] = gm::Vec2f( 1.0f,-1.0f);
	}
}


static float	getNoiseValue(const Noise &noise, float x, float y)
{
	// Put x in noise coordonates
	if (x >= noise.shape.x)
		x -= ((int)x / noise.shape.x) * noise.shape.x;
	else if (x < 0.0f)
	{
		x -= ((int)x / noise.shape.x) * noise.shape.x;
		x = noise.shape.x + x;
		if (x >= noise.shape.x)
			x -= noise.shape.x;
	}

	// Put y in noise coordonates
	if (y >= noise.shape.y)
		y -= ((int)y / noise.shape.y) * noise.shape.y;
	else if (y < 0.0f)
	{
		y -= ((int)y / noise.shape.y) * noise.shape.y;
		y = noise.shape.y + y;
		if (y >= noise.shape.y)
			y -= noise.shape.y;
	}

	// Get coordonates of point in noise.
	gm::Vec2f	pos(x, y);
	int			xi = (int)x;
	int			yi = (int)y;
	float		xf = x - (int)x;
	float		yf = y - (int)y;

	// Compute u and v for interpolations
	float	u = fade(xf);
	float	v = fade(yf);

	// Left up corner
	int	xLU = xi;
	int	yLU = yi;
	const gm::Vec2f	&noiseLU = noise.noise[xLU + yLU * noise.shape.x];
	const gm::Vec2f	vecLU = (pos - gm::Vec2f(xi, yi));
	float	valueLU = gm::dot(noiseLU, vecLU);

	// Left down corner
	int	xLD = xi;
	int	yLD = (yi + 1) % noise.shape.y;
	const gm::Vec2f	&noiseLD = noise.noise[xLD + yLD * noise.shape.x];
	const gm::Vec2f	vecLD = (pos - gm::Vec2f(xi, yi + 1));
	float	valueLD = gm::dot(noiseLD, vecLD);

	// Right down corner
	int	xRD = (xi + 1) % noise.shape.x;
	int	yRD = (yi + 1) % noise.shape.y;
	const gm::Vec2f	&noiseRD = noise.noise[xRD + yRD * noise.shape.x];
	const gm::Vec2f	vecRD = (pos - gm::Vec2f(xi + 1, yi + 1));
	float	valueRD = gm::dot(noiseRD, vecRD);

	// Right up corner
	int	xRU = (xi + 1) % noise.shape.x;
	int	yRU = yi;
	const gm::Vec2f	&noiseRU = noise.noise[xRU + yRU * noise.shape.x];
	const gm::Vec2f	vecRU = (pos - gm::Vec2f(xi + 1, yi));
	float	valueRU = gm::dot(noiseRU, vecRU);

	// Interpolate x axis
	float	valueU = gm::lerp(valueLU, valueRU, u);
	float	valueD = gm::lerp(valueLD, valueRD, u);

	// Interpolate y axis
	float	value = gm::lerp(valueU, valueD, v);

	return (value);
}


static float	fade(float value)
{
	return (value * value * value * (value * (value * 6 - 15) + 10));
}
