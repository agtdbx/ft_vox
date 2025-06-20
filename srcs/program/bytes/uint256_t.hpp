#ifndef UINT256_T_HPP
# define UINT256_T_HPP

# include <iostream>

/**
 * @brief Class for an unsigned interger of 256 bits.
 */
class uint256_t
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
	uint64_t	parts[4];
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of uint256_t class.
	 *
	 * @return The default uint256_t.
	 */
	uint256_t(void);
	/**
	 * @brief Copy constructor of uint256_t class.
	 *
	 * @param obj The uint256_t to copy.
	 *
	 * @return The uint256_t copied from parameter.
	 */
	uint256_t(const uint256_t &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of uint256_t class.
	 */
	~uint256_t();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//**** OPERATORS ***************************************************************
//---- Equal operators ---------------------------------------------------------
	/**
	 * @brief Copy operator of uint256_t class.
	 *
	 * @param obj The uint256_t to copy.
	 *
	 * @return The uint256_t copied from parameter.
	 */
	uint256_t	&operator=(const uint256_t &obj);
//---- Comparaison operators ---------------------------------------------------
	/**
	 * @brief Operator ==.
	 *
	 * @param obj The uint256_t to compare with.
	 *
	 * @return Comparaison result.
	 */
	bool	operator==(const uint256_t &obj) const;
	/**
	 * @brief Operator !=.
	 *
	 * @param obj The uint256_t to compare with.
	 *
	 * @return Comparaison result.
	 */
	bool	operator!=(const uint256_t &obj) const;
	/**
	 * @brief Operator <.
	 *
	 * @param obj The uint256_t to compare with.
	 *
	 * @return Comparaison result.
	 */
	bool	operator<(const uint256_t &obj) const;
	/**
	 * @brief Operator <=.
	 *
	 * @param obj The uint256_t to compare with.
	 *
	 * @return Comparaison result.
	 */
	bool	operator<=(const uint256_t &obj) const;
	/**
	 * @brief Operator >.
	 *
	 * @param obj The uint256_t to compare with.
	 *
	 * @return Comparaison result.
	 */
	bool	operator>(const uint256_t &obj) const;
	/**
	 * @brief Operator >=.
	 *
	 * @param obj The uint256_t to compare with.
	 *
	 * @return Comparaison result.
	 */
	bool	operator>=(const uint256_t &obj) const;
//---- Binary operators --------------------------------------------------------
//---- Unary operators ---------------------------------------------------------
	/**
	 * @brief Operator ++uint256_t.
	 *
	 * @return The ++uint256_t.
	 */
	uint256_t	&operator++(void);
	/**
	 * @brief Operator uint256_t++.
	 *
	 * @return The uint256_t++.
	 */
	uint256_t	operator++(int);
	/**
	 * @brief Operator --uint256_t.
	 *
	 * @return The --uint256_t.
	 */
	uint256_t	&operator--(void);
	/**
	 * @brief Operator uint256_t--.
	 *
	 * @return The uint256_t--.
	 */
	uint256_t	operator--(int);

//**** PUBLIC METHODS **********************************************************
//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Method to increment the number.
	 */
	void	increment(void);
	/**
	 * @brief Method to decrement the number.
	 */
	void	decrement(void);
};

//**** PUBLIC OPERTATORS *******************************************************
//---- Arithmetic operators ----------------------------------------------------
/**
 * @brief Operator +.
 *
 * @param b1 First number to add.
 * @param b2 Second number to add.
 *
 * @return b1 + b2.
 */
uint256_t	operator+(const uint256_t &b1, const uint256_t &b2);
/**
 * @brief Operator -.
 *
 * @param b1 First number to add.
 * @param b2 Second number to add.
 *
 * @return b1 - b2.
 */
uint256_t	operator-(const uint256_t &b1, const uint256_t &b2);
//---- Other operators ---------------------------------------------------------
/**
 * @brief Print operator.
 *
 * @return ostream with uint256_t print in it.
 */
std::ostream	&operator<<(std::ostream &os, const uint256_t &bytes);

//**** FUNCTIONS ***************************************************************

#endif
