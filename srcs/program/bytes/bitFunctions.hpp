#ifndef BIT_FUNCTIONS
# define BIT_FUNCTIONS

# include <program/bytes/uint256_t.hpp>

# include <cinttypes>

//**** FUNCTIONS ***************************************************************
/**
 * @brief Get the number of 1 before a 0. For 0010, it will return 1.
 *
 * @param bytes Number to trail.
 *
 * @return The number of 1 before a 0.
 */
int	trailing64Zero(uint64_t bytes);
/**
 * @brief Get the number of 1 before a 0. For 0010, it will return 1.
 *
 * @param bytes Number to trail.
 *
 * @return The number of 1 before a 0.
 */
int	trailing256Zero(const uint256_t &bytes);
/**
 * @brief Get the number of 0 before a 1. For 1000, it will return 3.
 *
 * @param bytes Number to trail.
 *
 * @return The number of 0 before a 1.
 */
int	trailing64One(uint64_t bytes);
/**
 * @brief Get the number of 0 before a 1. For 1000, it will return 3.
 *
 * @param bytes Number to trail.
 *
 * @return The number of 0 before a 1.
 */
int	trailing256One(const uint256_t &bytes);
/**
 * @brief Create a mask of parameter length. For length = 3, mask = 0111.
 *
 * @param length Number of ones in the mask.
 *
 * @return The mask.
 */
uint64_t	create64LengthMask(int length);
/**
 * @brief Create a mask of parameter length. For length = 3, mask = 0111.
 *
 * @param length Number of ones in the mask.
 *
 * @return The mask.
 */
uint256_t	create256LengthMask(int length);
/**
 * @brief Reverse the number. 1110 will become 0111.
 *
 * @param bytes Number to reverse.
 *
 * @return Number reversed.
 */
uint32_t	reverse32Bytes(uint32_t bytes);
/**
 * @brief Reverse the number. 1110 will become 0111.
 *
 * @param bytes Number to reverse.
 *
 * @return Number reversed.
 */
uint64_t	reverse64Bytes(uint64_t bytes);
/**
 * @brief Reverse the number. 1110 will become 0111.
 *
 * @param bytes Number to reverse.
 *
 * @return Number reversed.
 */
uint256_t	reverse256Bytes(const uint256_t &bytes);

#endif
