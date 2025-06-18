#ifndef CUBE_BITMAP_HPP
# define CUBE_BITMAP_HPP

# include <define.hpp>

struct CubeBitmap
{
	int32_t	axisX[CHUNK_MASK_SIZE]; // id = z + y * SIZE, 1 << x
	int32_t	axisZ[CHUNK_MASK_SIZE]; // id = x + y * SIZE, 1 << z

	bool	getX(int x, int y, int z)
	{
		return (this->axisX[z + y * CHUNK_SIZE] & (0b1 << x));
	}

	bool	getZ(int x, int y, int z)
	{
		return (this->axisZ[x + y * CHUNK_SIZE] & (0b1 << z));
	}

	void	setX(int x, int y, int z, bool cube)
	{
		int		id = z + y * CHUNK_SIZE;
		int32_t	mask = 0b1 << x;
		if ((this->axisX[id] & mask) == cube)
			return ;

		if (cube)
			this->axisX[id] += mask;
		else
			this->axisX[id] -= mask;
	}

	void	setZ(int x, int y, int z, bool cube)
	{
		int		id = x + y * CHUNK_SIZE;
		int32_t	mask = 0b1 << z;
		if ((this->axisZ[id] & mask) == cube)
			return ;

		if (cube)
			this->axisZ[id] += mask;
		else
			this->axisZ[id] -= mask;
	}

	void	set(int x, int y, int z, bool cube)
	{
		int		idY = y * CHUNK_SIZE;
		int32_t	maskX = 0b1 << x;
		int32_t	maskZ = 0b1 << z;
		if ((this->axisX[z + idY] & maskX) == cube)
			return ;

		if (cube)
		{
			this->axisX[z + idY] += maskX;
			this->axisZ[x + idY] += maskZ;
		}
		else
		{
			this->axisX[z + idY] -= maskX;
			this->axisZ[x + idY] -= maskZ;
		}
	}

	CubeBitmap	&operator=(const CubeBitmap &obj)
	{
		if (this == &obj)
			return (*this);

		for (int i = 0; i < CHUNK_MASK_SIZE; i++)
		{
			this->axisX[i] = obj.axisX[i];
			this->axisZ[i] = obj.axisZ[i];
		}

		return (*this);
	}
};

#endif
