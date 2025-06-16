#include <program/map/PerlinNoise.hpp>

//**** STATIC Variable *************************************************

const gm::Vec2f	vecLeftTop = {1.0, 1.0};
const gm::Vec2f	vecRightTop = {-1.0, 1.0};
const gm::Vec2f	vecRightBot = {-1.0, -1.0};
const gm::Vec2f	vecLeftBot = {1.0, -1.0};

//**** STATIC FUNCTIONS *************************************************

static void	generate_perlin_noise(
				int *perlin_noise,
                int seed,
				int perlin_noise_size)
{
	int	shuffleIndex, tmp;

	srand(seed);
	for (int i = 0; i < perlin_noise_size; i++)
		perlin_noise[i] = i;

	for (int i = 0; i < perlin_noise_size; i++)
	{
		shuffleIndex = rand() % perlin_noise_size;
		tmp = perlin_noise[i];
		perlin_noise[i] = perlin_noise[shuffleIndex];
		perlin_noise[shuffleIndex] = tmp;
	}

	for (int i = 0; i < perlin_noise_size; i++)
		perlin_noise[i + perlin_noise_size] = perlin_noise[i];
}


static float	fade(
					float value)
{
	return (((6 * value -  15) *  value + 10) *  value * value * value);
}

static float	lerp(
					float start,
					float end,
					float ratio)
{
	return (start + (end - start) * ratio);
}

// ########################## CLASS #################################################3

//---- Constructors ------------------------------------------------------------
PerlinNoise::PerlinNoise(void)
{
	this->perlin_noise_size = PERLIN_NOISE_SIZE;
	this->perlin_noise_list_size = this->perlin_noise_size * 2;
	this->perlin_noise_mask = this->perlin_noise_list_size - 1;
	this->perlin_noise.resize(this->perlin_noise_list_size);
    generate_perlin_noise(this->perlin_noise.data(), SEED, this->perlin_noise_size);
}

PerlinNoise::PerlinNoise(int seed, int perlin_noise_size)
{
	this->perlin_noise_size = perlin_noise_size;
	this->perlin_noise_list_size = this->perlin_noise_size * 2;
	this->perlin_noise_mask = this->perlin_noise_list_size - 1;
	this->perlin_noise.resize(this->perlin_noise_list_size);
    generate_perlin_noise(this->perlin_noise.data(), seed, this->perlin_noise_size);
}

//---- Destructor ------------------------------------------------------------
PerlinNoise::~PerlinNoise()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
float PerlinNoise::getNoise(float x, float y)
{  
	int			pnX, pnY;
	float		x_ratio, y_ratio;
	gm::Vec2f		vecLeftTop, vecRightTop, vecLeftBot, vecRightBot;
	float		valueLeftTop, valueRightTop, valueLeftBot, valueRightBot,
                dotLeftTop, dotRightTop, dotLeftBot, dotRightBot,
                topValue, botValue;

    x *= this->perlin_noise_size;
	y *= this->perlin_noise_size;

	pnX = ((int)x) & this->perlin_noise_mask;
	pnY = ((int)y) & this->perlin_noise_mask;

	x_ratio = x - ((int)x);
	y_ratio = y - ((int)y);

	vecLeftTop = {x_ratio, y_ratio - 1.0f};
	vecRightTop = {x_ratio - 1.0f, y_ratio - 1.0f};
	vecLeftBot = {x_ratio, y_ratio};
	vecRightBot = {x_ratio - 1.0f, y_ratio};

	valueLeftTop = this->perlin_noise[this->perlin_noise[pnX] + pnY + 1];
	valueRightTop = this->perlin_noise[this->perlin_noise[pnX + 1] + pnY + 1];
	valueLeftBot = this->perlin_noise[this->perlin_noise[pnX] + pnY];
	valueRightBot = this->perlin_noise[this->perlin_noise[pnX + 1] + pnY];

	dotLeftTop = gm::dot(vecLeftTop, this->get_constant_vector(valueLeftTop));
	dotRightTop = gm::dot(vecRightTop, this->get_constant_vector(valueRightTop));
	dotLeftBot = gm::dot(vecLeftBot, this->get_constant_vector(valueLeftBot));
	dotRightBot = gm::dot(vecRightBot, this->get_constant_vector(valueRightBot));

	x_ratio = fade(x_ratio);
	y_ratio = fade(y_ratio);

	topValue = lerp(dotLeftTop, dotRightTop, x_ratio);
	botValue = lerp(dotLeftBot, dotRightBot, x_ratio);

    float   perlinValue = lerp(botValue, topValue, y_ratio);
    float   perlinNormalize = (perlinValue + 1.0f) / 2.0f;

	return (perlinNormalize);
}

//**** PRIVATE METHODS *********************************************************
gm::Vec2f	PerlinNoise::get_constant_vector(
					int value)
{
	static	gm::Vec2f	vecLeftTop = {1.0, 1.0};
	static	gm::Vec2f	vecRightTop = {-1.0, 1.0};
	static	gm::Vec2f	vecRightBot = {-1.0, -1.0};
	static	gm::Vec2f	vecLeftBot = {1.0, -1.0};
	int				height;

	height = value & 3;
	if (height == 0)
		return (vecLeftTop);
	else if (height == 1)
		return (vecRightTop);
	else if (height == 2)
		return (vecRightBot);
	return (vecLeftBot);
}