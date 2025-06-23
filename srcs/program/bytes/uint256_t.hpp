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
	/**
	 * @brief Constructor of uint256_t class.
	 *
	 * @param value The default value as an uint64.
	 *
	 * @return The uint256_t with default value of parameter.
	 */
	uint256_t(uint64_t value);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of uint256_t class.
	 */
	~uint256_t();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Get the status of bit at the offset.
	 *
	 * @param offset The offset of the bit.
	 *
	 * @return The status of the bit, or false in case of bad offset.
	 */
	bool	get(int offset);
//---- Setters -----------------------------------------------------------------
	/**
	 * @brief Get the status of bit at the offset.
	 *
	 * @param offset The offset of the bit.
	 * @param status The new status of the bit.
	 */
	void	set(int offset, bool status);

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
	/**
	 * @brief Operator += of uint256_t class.
	 *
	 * @param obj The uint256_t to add.
	 *
	 * @return The uint256_t + obj.
	 */
	uint256_t	&operator+=(const uint256_t &obj);
	/**
	 * @brief Operator -= of uint256_t class.
	 *
	 * @param obj The uint256_t to sub.
	 *
	 * @return The uint256_t - obj.
	 */
	uint256_t	&operator-=(const uint256_t &obj);
		/**
	 * @brief And bit operation.
	 *
	 * @param obj The uint256_t to and with.
	 *
	 * @return The uint256_t & obj.
	 */
	uint256_t	&operator&=(const uint256_t &obj);
	/**
	 * @brief Or bit operation.
	 *
	 * @param obj The uint256_t to or with.
	 *
	 * @return The uint256_t | obj.
	 */
	uint256_t	&operator|=(const uint256_t &obj);
	/**
	 * @brief Xor bit operation.
	 *
	 * @param obj The uint256_t to xor with.
	 *
	 * @return The uint256_t ^ obj.
	 */
	uint256_t	&operator^=(const uint256_t &obj);
	/**
	 * @brief Not bit operation.
	 *
	 * @param shift How many bits to shift.
	 *
	 * @return Bit shift to left.
	 */
	uint256_t	&operator>>=(int shift);
	/**
	 * @brief Not bit operation.
	 *
	 * @param shift How many bits to shift.
	 *
	 * @return Bit shift to right.
	 */
	uint256_t	&operator<<=(int shift);
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
	/**
	 * @brief And bit operation.
	 *
	 * @param obj The uint256_t to and with.
	 *
	 * @return uint256_t & obj.
	 */
	uint256_t	operator&(const uint256_t &obj) const;
	/**
	 * @brief Or bit operation.
	 *
	 * @param obj The uint256_t to or with.
	 *
	 * @return uint256_t | obj.
	 */
	uint256_t	operator|(const uint256_t &obj) const;
	/**
	 * @brief Xor bit operation.
	 *
	 * @param obj The uint256_t to xor with.
	 *
	 * @return uint256_t ^ obj.
	 */
	uint256_t	operator^(const uint256_t &obj) const;
	/**
	 * @brief Reverse bit operation (010 -> 101).
	 *
	 * @return Reverse bit value.
	 */
	uint256_t	operator~(void) const;
	/**
	 * @brief Not bit operation.
	 *
	 * @param shift How many bits to shift.
	 *
	 * @return Bit shift to left.
	 */
	uint256_t	operator>>(int shift) const;
	/**
	 * @brief Not bit operation.
	 *
	 * @param shift How many bits to shift.
	 *
	 * @return Bit shift to right.
	 */
	uint256_t	operator<<(int shift) const;
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
