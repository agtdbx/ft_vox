#include <program/bytes/bitFunctions.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** FUNCTIONS ***************************************************************

int	trailing64Zero(uint64_t bytes)
{
	if (bytes == 0)
		return (64);
	return (__builtin_ctzll(bytes));
}


int	trailing256Zero(const uint256_t &bytes)
{
	int	res = 0;

	for (int i = 3; i >= 0; i--)
	{
		if (bytes.parts[i] == 0ull)
			res += 64;
		else
			return (res + __builtin_ctzll(bytes.parts[i]));
	}

	return (res);
}


int	trailing64One(uint64_t bytes)
{
	if (bytes == UINT64_MAX)
		return (64);
	return (__builtin_ctzll(~bytes));
}


int	trailing256One(const uint256_t &bytes)
{
	int	res = 0;

	for (int i = 3; i >= 0; i--)
	{
		if (bytes.parts[i] == UINT64_MAX)
			res += 64;
		else
			return (res + __builtin_ctzll(~bytes.parts[i]));
	}

	return (res);
}


uint64_t	create64LengthMask(int length)
{
	if (length <= 0)
		return (0ull);

	if (length >= 64)
		return (UINT64_MAX);

	return ((1ull << length) - 1ull);
}


uint256_t	create256LengthMask(int length)
{
	uint256_t	result;

	if (length <= 0)
		return (result);

	if (length >= 256)
	{
		result.parts[0] = UINT64_MAX;
		result.parts[1] = UINT64_MAX;
		result.parts[2] = UINT64_MAX;
		result.parts[3] = UINT64_MAX;
		return (result);
	}

	for (int i = 3; i >= 0; i--)
	{
		if (length < 64)
		{
			result.parts[i] = (1ull << length) - 1ull;
			return (result);
		}
		length -= 64;
		result.parts[i] = UINT64_MAX;
	}

	return (result);
}


uint32_t reverse32Bytes(uint32_t bytes)
{
	bytes = ((bytes >> 1 ) & 0x55555555) | ((bytes & 0x55555555) << 1 );
	bytes = ((bytes >> 2 ) & 0x33333333) | ((bytes & 0x33333333) << 2 );
	bytes = ((bytes >> 4 ) & 0x0F0F0F0F) | ((bytes & 0x0F0F0F0F) << 4 );
	bytes = ((bytes >> 8 ) & 0x00FF00FF) | ((bytes & 0x00FF00FF) << 8 );
	bytes = ((bytes >> 16) & 0x0000FFFF) | ((bytes & 0x0000FFFF) << 16);

	return (bytes);
}


uint64_t reverse64Bytes(uint64_t bytes)
{
	bytes = ((bytes >> 1 ) & 0x5555555555555555ULL) | ((bytes & 0x5555555555555555ULL) << 1 );
	bytes = ((bytes >> 2 ) & 0x3333333333333333ULL) | ((bytes & 0x3333333333333333ULL) << 2 );
	bytes = ((bytes >> 4 ) & 0x0F0F0F0F0F0F0F0FULL) | ((bytes & 0x0F0F0F0F0F0F0F0FULL) << 4 );
	bytes = ((bytes >> 8 ) & 0x00FF00FF00FF00FFULL) | ((bytes & 0x00FF00FF00FF00FFULL) << 8 );
	bytes = ((bytes >> 16) & 0x0000FFFF0000FFFFULL) | ((bytes & 0x0000FFFF0000FFFFULL) << 16);
	bytes = (bytes >> 32) | (bytes << 32);

	return (bytes);
}


uint256_t	reverse256Bytes(const uint256_t &bytes)
{
	uint256_t	result;

	result.parts[0] = reverse64Bytes(bytes.parts[3]);
	result.parts[1] = reverse64Bytes(bytes.parts[2]);
	result.parts[2] = reverse64Bytes(bytes.parts[1]);
	result.parts[3] = reverse64Bytes(bytes.parts[0]);

	return (result);
}

//**** STATIC FUNCTIONS ********************************************************

