#include <program/map/PerlinNoise.hpp>


static void	generate_perlin_noise(
				int *perlin_noise,
                int seed)
{
	int	shuffleIndex, tmp;

	srand(seed);
	for (int i = 0; i < PERLIN_NOISE_SIZE; i++)
		perlin_noise[i] = i;

	for (int i = 0; i < PERLIN_NOISE_SIZE; i++)
	{
		shuffleIndex = rand() % PERLIN_NOISE_SIZE;
		tmp = perlin_noise[i];
		perlin_noise[i] = perlin_noise[shuffleIndex];
		perlin_noise[shuffleIndex] = tmp;
	}

	for (int i = 0; i < PERLIN_NOISE_SIZE; i++)
		perlin_noise[i + PERLIN_NOISE_SIZE] = perlin_noise[i];
}

static float   dot(const gm::Vec2f &v1, const gm::Vec2f &v2)
{
    return (v1.x * v2.x + v1.y * v2.y);
}

static gm::Vec2f	get_constant_vector(
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


PerlinNoise::PerlinNoise(void)
{
    generate_perlin_noise(this->perlin_noiseGeneration, this->seed);
    generate_perlin_noise(this->perlin_noiseTerrain, this->seedTerrain);
    generate_perlin_noise(this->perlin_noiseBiome, this->seedBiome);
}

PerlinNoise::~PerlinNoise()
{

}

float getNoise(float x, float y, int perlin_noisef[PERLIN_NOISE_LIST_SIZE])
{  
	int			pnX;
	int			pnY;
	float		x_ratio;
	float		y_ratio;
	gm::Vec2f		vecLeftTop;
	gm::Vec2f		vecRightTop;
	gm::Vec2f		vecLeftBot;
	gm::Vec2f		vecRightBot;
	float		valueLeftTop;
	float		valueRightTop;
	float		valueLeftBot;
	float		valueRightBot;
	float		dotLeftTop;
	float		dotRightTop;
	float		dotLeftBot;
	float		dotRightBot;
	float		topValue;
	float		botValue;

    x *= PERLIN_NOISE_SIZE;
	y *= PERLIN_NOISE_SIZE;

	pnX = ((int)x) & PERLIN_NOISE_MASK;
	pnY = ((int)y) & PERLIN_NOISE_MASK;

	x_ratio = x - ((int)x);
	y_ratio = y - ((int)y);

	vecLeftTop = {x_ratio, y_ratio - 1.0f};
	vecRightTop = {x_ratio - 1.0f, y_ratio - 1.0f};
	vecLeftBot = {x_ratio, y_ratio};
	vecRightBot = {x_ratio - 1.0f, y_ratio};

	valueLeftTop = perlin_noisef[perlin_noisef[pnX] + pnY + 1];
	valueRightTop = perlin_noisef[perlin_noisef[pnX + 1] + pnY + 1];
	valueLeftBot = perlin_noisef[perlin_noisef[pnX] + pnY];
	valueRightBot = perlin_noisef[perlin_noisef[pnX + 1] + pnY];

	dotLeftTop = dot(vecLeftTop, get_constant_vector(valueLeftTop));
	dotRightTop = dot(vecRightTop, get_constant_vector(valueRightTop));
	dotLeftBot = dot(vecLeftBot, get_constant_vector(valueLeftBot));
	dotRightBot = dot(vecRightBot, get_constant_vector(valueRightBot));

	x_ratio = fade(x_ratio);
	y_ratio = fade(y_ratio);

	topValue = lerp(dotLeftTop, dotRightTop, x_ratio);
	botValue = lerp(dotLeftBot, dotRightBot, x_ratio);

    float   perlinValue = lerp(botValue, topValue, y_ratio);
    float   perlinNormalize = (perlinValue + 1.0f) / 2.0f;

	return (perlinNormalize * 32.0f + 48.0f);
}

float PerlinNoise::goToNoise(float x, float y, int type)
{
    if (type == GENERATION)
        return (getNoise(x, y, this->perlin_noiseGeneration));
    else if (type == BIOME)
        return (getNoise(x, y, this->perlin_noiseBiome));
    else if (type == TERRAIN)
        return (getNoise(x, y, this->perlin_noiseTerrain));
    else
        return(-1);
}
