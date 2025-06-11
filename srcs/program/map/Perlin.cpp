#include <program/map/Perlin.hpp>

const size_t    SEED = 42;

const int PERLIN_NOISE_SIZE = 64;
const int PERLIN_NOISE_LIST_SIZE = PERLIN_NOISE_SIZE * 2;
const int PERLIN_NOISE_MASK = PERLIN_NOISE_LIST_SIZE - 1;

static void		generate_perlin_noise(
					int *perlin_noise);
static gm::Vec2f	get_constant_vector(
					int value);
static float	fade(
					float value);
static float	lerp(
					float start,
					float end,
					float ratio);


float   dot(const gm::Vec2f &v1, const gm::Vec2f &v2)
{
    return (v1.x * v2.x + v1.y * v2.y);
}


float	perlin(
			float x,
			float y)
{
	static bool	perlin_noise_generated = false;
	static int	perlin_noise[PERLIN_NOISE_LIST_SIZE] = {0};
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

	if (!perlin_noise_generated)
	{
		generate_perlin_noise(perlin_noise);
		perlin_noise_generated = true;
	}

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

	valueLeftTop = perlin_noise[perlin_noise[pnX] + pnY + 1];
	valueRightTop = perlin_noise[perlin_noise[pnX + 1] + pnY + 1];
	valueLeftBot = perlin_noise[perlin_noise[pnX] + pnY];
	valueRightBot = perlin_noise[perlin_noise[pnX + 1] + pnY];

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

static void	generate_perlin_noise(
				int *perlin_noise)
{
	int	shuffleIndex, tmp;

	srand(SEED);
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










































//TODO delete this old perlin function
/*static gm::Vec2f randomGradient(int ix, int iy)
{
	// No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = ix, b = iy;
    a *= 3284157443;
 
    b ^= a << s | a >> (w - s);
    b *= 1911520717;
 
    a ^= b << s | b >> (w - s);
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
	gm::Vec2f v;
    // Create the vector from the angle
    v.x = sin(random);
    v.y = cos(random);
    return v;
}

static float dotGridGradient(int ix, int iy, float x, float y)
{
    // Get gradient from integer coordinates
    gm::Vec2f gradient = randomGradient(ix, iy);
 
    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;
 
    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}

static float interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float perlin(float x, float y)
{
    
    // Determine grid cell corner coordinates
    int x0 = (int)x; 
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;
 
    // Compute Interpolation weights
    float sx = x - (float)x0;
    float sy = y - (float)y0;
    
    // Compute and interpolate top two corners
    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);
 
    // Compute and interpolate bottom two corners
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);
 
    // Final step: interpolate between the two previously interpolated values, now in y
    float value = interpolate(ix0, ix1, sy);
    
	value = value + 1;
	value = value / 2;
	value = value * 64;
    return value;
}*/