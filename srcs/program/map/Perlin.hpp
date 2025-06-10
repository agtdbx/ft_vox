#ifndef PERLIN_HPP
# define PERLIN_HPP

# include <gmath.hpp>


    /**
    * @brief Call the function to determine the max height of a column with perlin noise
    *
    * @param x The X coordinate of a block in a chunk
    *
    * @param y The Z coordinate of a block in a chunk
    *
    * @return The max height of the column
    */
    float perlin(float x, float y, int seed);

#endif