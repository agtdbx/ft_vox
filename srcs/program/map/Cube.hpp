#ifndef CUBE_HPP
# define CUBE_HPP

enum	Cube : uint8_t
{
	// Basic cube
	CUBE_GRASS = 0,
	CUBE_DIRT,
	CUBE_STONE,

	// Snow
	CUBE_SNOW,
	CUBE_ICE,

	// Desert
	CUBE_SAND,
	CUBE_RED_SAND,
	CUBE_TERA_BROWN,
	CUBE_TERA_RED,
	CUBE_TERA_WHITE,
	CUBE_TERA_YELLOW,

	// Underground
	CUBE_IRON,
	CUBE_DIAMOND,
	CUBE_BEDROCK,

	// Tree
	CUBE_WOOD,
	CUBE_LEAVES,

	// Liquids
	CUBE_WATER,
	CUBE_LAVA,

	// Air
	CUBE_AIR,
};

#endif
