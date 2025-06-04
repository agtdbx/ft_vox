#ifndef STRING_HPP
# define STRING_HPP

# include <string>
# include <vector>

/**
 * @brief Split a string be a character.
 *
 * @param s The string to split.
 * @param c The char to use as separator.
 *
 * @return A vector of string, split by c.
 */
std::vector<std::string> split(const std::string &s, const char c);
/**
 * @brief Transform a string to an int.
 *
 * @param s The string to transform.
 * @param res The int value get by the char.
 *
 * @return False in case of error, true else.
 */
bool	strToInt(const std::string &s, int &res);
/**
 * @brief Transform a string to a float.
 *
 * @param s The string to transform.
 * @param res The float value get by the char.
 *
 * @return False in case of error, true else.
 */
bool	strToFloat(const std::string &s, float &res);
/**
 * @brief Count the number of occurrences of the char into the string.
 *
 * @param s The string where count c.
 * @param c The char to count.
 *
 * @return Number of occurrences of c in s.
 */
int	nbOccurences(const std::string &s, const char c);
#endif
