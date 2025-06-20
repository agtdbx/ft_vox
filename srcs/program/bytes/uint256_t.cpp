// #include <program/bytes/uint256_t.hpp> // Todo : Reput correct include
#include "srcs/program/bytes/uint256_t.hpp"

//**** STATIC VARIABLES DEFINE *************************************************

const	uint64_t	ONE_LEFT =  0b1000000000000000000000000000000000000000000000000000000000000000;
const	uint64_t	ZERO_LEFT = 0b0111111111111111111111111111111111111111111111111111111111111111;

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

uint256_t::uint256_t(void)
{
	this->parts[0] = 0ull;
	this->parts[1] = 0ull;
	this->parts[2] = 0ull;
	this->parts[3] = 0ull;
}


uint256_t::uint256_t(const uint256_t &obj)
{
	this->parts[0] = obj.parts[0];
	this->parts[1] = obj.parts[1];
	this->parts[2] = obj.parts[2];
	this->parts[3] = obj.parts[3];
}

//---- Destructor --------------------------------------------------------------

uint256_t::~uint256_t()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//**** OPERATORS ***************************************************************
//---- Equal operators ---------------------------------------------------------

uint256_t	&uint256_t::operator=(const uint256_t &obj)
{
	if (this == &obj)
		return (*this);

	this->parts[0] = obj.parts[0];
	this->parts[1] = obj.parts[1];
	this->parts[2] = obj.parts[2];
	this->parts[3] = obj.parts[3];

	return (*this);
}

//---- Comparaison operators ---------------------------------------------------

bool	uint256_t::operator==(const uint256_t &obj) const
{
	return (this->parts[0] == obj.parts[0] &&
				this->parts[1] == obj.parts[1] &&
				this->parts[2] == obj.parts[2] &&
				this->parts[3] == obj.parts[3]);
}


bool	uint256_t::operator!=(const uint256_t &obj) const
{
	return (this->parts[0] != obj.parts[0] ||
				this->parts[1] != obj.parts[1] ||
				this->parts[2] != obj.parts[2] ||
				this->parts[3] != obj.parts[3]);
}


bool	uint256_t::operator<(const uint256_t &obj) const
{
	if (this->parts[0] != obj.parts[0])
		return (this->parts[0] < obj.parts[0]);

	if (this->parts[1] != obj.parts[1])
		return (this->parts[1] < obj.parts[1]);

	if (this->parts[2] != obj.parts[2])
		return (this->parts[2] < obj.parts[2]);

	if (this->parts[3] != obj.parts[3])
		return (this->parts[3] < obj.parts[3]);

	return (false);
}


bool	uint256_t::operator<=(const uint256_t &obj) const
{
	if (this->parts[0] != obj.parts[0])
		return (this->parts[0] < obj.parts[0]);

	if (this->parts[1] != obj.parts[1])
		return (this->parts[1] < obj.parts[1]);

	if (this->parts[2] != obj.parts[2])
		return (this->parts[2] < obj.parts[2]);

	if (this->parts[3] != obj.parts[3])
		return (this->parts[3] < obj.parts[3]);

	return (true);
}


bool	uint256_t::operator>(const uint256_t &obj) const
{
	if (this->parts[0] != obj.parts[0])
		return (this->parts[0] > obj.parts[0]);

	if (this->parts[1] != obj.parts[1])
		return (this->parts[1] > obj.parts[1]);

	if (this->parts[2] != obj.parts[2])
		return (this->parts[2] > obj.parts[2]);

	if (this->parts[3] != obj.parts[3])
		return (this->parts[3] > obj.parts[3]);

	return (false);
}


bool	uint256_t::operator>=(const uint256_t &obj) const
{
	if (this->parts[0] != obj.parts[0])
		return (this->parts[0] > obj.parts[0]);

	if (this->parts[1] != obj.parts[1])
		return (this->parts[1] > obj.parts[1]);

	if (this->parts[2] != obj.parts[2])
		return (this->parts[2] > obj.parts[2]);

	if (this->parts[3] != obj.parts[3])
		return (this->parts[3] > obj.parts[3]);

	return (true);
}

//---- Binary operators --------------------------------------------------------
//---- Unary operators ---------------------------------------------------------

uint256_t	&uint256_t::operator++(void)
{
	this->increment();

	return (*this);
}


uint256_t	uint256_t::operator++(int)
{
	uint256_t	copy(*this);

	this->increment();

	return (copy);
}


uint256_t	&uint256_t::operator--(void)
{
	this->decrement();

	return (*this);
}


uint256_t	uint256_t::operator--(int)
{
	uint256_t	copy(*this);

	this->decrement();

	return (copy);
}

//**** PUBLIC METHODS **********************************************************
//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	uint256_t::increment(void)
{
	if (this->parts[3] != UINT64_MAX)
	{
		this->parts[3]++;
		return ;
	}
	this->parts[3] = 0ull;

	if (this->parts[2] != UINT64_MAX)
	{
		this->parts[2]++;
		return ;
	}
	this->parts[2] = 0ull;

	if (this->parts[1] != UINT64_MAX)
	{
		this->parts[1]++;
		return ;
	}
	this->parts[1] = 0ull;

	if (this->parts[0] != UINT64_MAX)
	{
		this->parts[0]++;
		return ;
	}
	this->parts[0] = 0ull;
}


void	uint256_t::decrement(void)
{
	if (this->parts[3] != 0ull)
	{
		this->parts[3]--;
		return ;
	}
	this->parts[3] = UINT64_MAX;

	if (this->parts[2] != 0ull)
	{
		this->parts[2]--;
		return ;
	}
	this->parts[2] = UINT64_MAX;

	if (this->parts[1] != 0ull)
	{
		this->parts[1]--;
		return ;
	}
	this->parts[1] = UINT64_MAX;

	if (this->parts[0] != 0ull)
	{
		this->parts[0]--;
		return ;
	}
	this->parts[0] = UINT64_MAX;
}

//**** PUBLIC OPERTATORS *******************************************************
//---- Arithmetic operators ----------------------------------------------------

uint256_t	operator+(const uint256_t &b1, const uint256_t &b2)
{
	uint256_t	result;
	uint64_t	carry = 0ull;

	for (int i = 3; i >= 0; --i)
	{
		uint64_t a = b1.parts[i];
		uint64_t b = b2.parts[i];

		uint64_t sum = a + b + carry;
		carry = (sum < a || (carry && sum == a)) ? 1 : 0;

		result.parts[i] = sum;
	}

	return (result);
}


uint256_t	operator-(const uint256_t &b1, const uint256_t &b2)
{
	uint256_t result;
	uint64_t borrow = 0;

	for (int i = 3; i >= 0; --i)
	{
		uint64_t a = b1.parts[i];
		uint64_t b = b2.parts[i];

		uint64_t sub = a - b - borrow;

		// Borrow if a < b + borrow
		if (a < b + borrow)
			borrow = 1;
		else
			borrow = 0;

		result.parts[i] = sub;
	}

	return (result);
}

//---- Other operators ---------------------------------------------------------

std::ostream	&operator<<(std::ostream &os, const uint256_t &bytes)
{
	os << "part 0 : ";
	for (int i = 63; i >= 0; i--)
	{
		if (bytes.parts[0] & 1ull << i)
			os << "1";
		else
			os << "0";
	}

	os << "\npart 1 : ";
	for (int i = 63; i >= 0; i--)
	{
		if (bytes.parts[1] & 1ull << i)
			os << "1";
		else
			os << "0";
	}

	os << "\npart 2 : ";
	for (int i = 63; i >= 0; i--)
	{
		if (bytes.parts[2] & 1ull << i)
			os << "1";
		else
			os << "0";
	}

	os << "\npart 3 : ";
	for (int i = 63; i >= 0; i--)
	{
		if (bytes.parts[3] & 1ull << i)
			os << "1";
		else
			os << "0";
	}

	return (os);
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
