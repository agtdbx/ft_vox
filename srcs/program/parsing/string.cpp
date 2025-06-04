#include <program/parsing/string.hpp>

#include <limits>

std::vector<std::string> split(const std::string &s, const char c)
{
	int							i, j;
	std::vector<std::string>	res;

	i = 0;
	while (s[i] != '\0')
	{
		while (s[i] == c)
			i++;
		if (s[i] == '\0')
			break ;
		j = 0;
		while (s[i + j] != c && s[i + j] != '\0')
			j++;
		res.push_back(s.substr(i, j));
		i += j;
	}
	return (res);
}


bool	strToInt(const std::string &s, int &res)
{
	if (s.empty())
		return (false);

	size_t	len = s.size();

	size_t	start = 0;
	size_t	tmp = 0;

	if (s[0] == '+' || s[0] == '-')
		start++;

	// Check for invalid caracter
	tmp = start;
	while (tmp < len)
	{
		if (s[tmp] < '0' || s[tmp] > '9')
			return (false);
		tmp++;
	}

	// Check overflow by length
	if (tmp - start > 10)
		return (false);

	long	value = std::stol(s.c_str());

	// Check overflow by value
	if (value > INT32_MAX || value < INT32_MIN)
		return (false);

	// Value is an integer
	res = static_cast<int>(value);
	return (true);
}

bool	strToFloat(const std::string &s, float &res)
{
	if (s.empty())
		return (false);

	size_t	len = s.size();

	size_t	start = 0;
	size_t	tmp = 0;
	bool	dot = false;

	if (s[0] == '+' || s[0] == '-')
		start++;

	// Check for invalid caracter
	tmp = start;
	while (tmp < len)
	{
		if (s[tmp] == '.')
		{
			if (!dot && tmp > start)
				dot = true;
			else
				return (false);
		}
		else if (s[tmp] < '0' || s[tmp] > '9')
			return (false);

		tmp++;
	}

	// Check overflow by length
	if (tmp - start > 20)
		return (false);

	double	value = std::stod(s.c_str());

	// Check overflow by value
	if (value > std::numeric_limits<float>::max() || value < -std::numeric_limits<float>::max())
		return (false);

	// Value is an integer
	res = static_cast<float>(value);
	return (true);
}


int	nbOccurences(const std::string &s, const char c)
{
	int	occurence = 0;

	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] == c)
			occurence++;
	}
	return (occurence);
}
