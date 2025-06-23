#include <program/bytes/uint256_t.hpp>

#include <program/bytes/bitFunctions.hpp>

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

bool	uint256_t::get(int offset)
{
	if (offset < 0 || offset >= 256)
		return (false);

	if (offset < 64)
		return (this->parts[3] & (1ull << offset));

	if (offset < 128)
		return (this->parts[2] & (1ull << (offset - 64)));

	if (offset < 192)
		return (this->parts[1] & (1ull << (offset - 128)));

	return (this->parts[0] & (1ull << (offset - 192)));
}

//---- Setters -----------------------------------------------------------------

void	uint256_t::set(int offset, bool status)
{
	if (offset < 0 || offset >= 256)
		return ;


	if (offset < 64)
	{
		const uint64_t	mask = 1ull << offset;

		if ((this->parts[3] & mask) == status)
			return ;

		if (status)
			this->parts[3] += mask;
		else
			this->parts[3] -= mask;
	}
	else if (offset < 128)
	{
		const uint64_t	mask = 1ull << (offset - 64);

		if ((this->parts[2] & mask) == status)
			return ;

		if (status)
			this->parts[2] += mask;
		else
			this->parts[2] -= mask;
	}
	else if (offset < 192)
	{
		const uint64_t	mask = 1ull << (offset - 128);

		if ((this->parts[1] & mask) == status)
			return ;

		if (status)
			this->parts[1] += mask;
		else
			this->parts[1] -= mask;
	}
	else
	{
		const uint64_t	mask = 1ull << (offset - 192);

		if ((this->parts[0] & mask) == status)
			return ;

		if (status)
			this->parts[0] += mask;
		else
			this->parts[0] -= mask;
	}
}

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


uint256_t	&uint256_t::operator+=(const uint256_t &obj)
{
	uint64_t	carry = 0ull;

	for (int i = 3; i >= 0; --i)
	{
		uint64_t a = this->parts[i];
		uint64_t b = obj.parts[i];

		uint64_t sum = a + b + carry;
		carry = (sum < a || (carry && sum == a)) ? 1 : 0;

		this->parts[i] = sum;
	}

	return (*this);
}


uint256_t	&uint256_t::operator-=(const uint256_t &obj)
{
	uint64_t borrow = 0;

	for (int i = 3; i >= 0; --i)
	{
		uint64_t a = this->parts[i];
		uint64_t b = obj.parts[i];

		uint64_t sub = a - b - borrow;

		// Borrow if a < b + borrow
		if (a < b + borrow)
			borrow = 1;
		else
			borrow = 0;

		this->parts[i] = sub;
	}

	return (*this);
}


uint256_t	&uint256_t::operator&=(const uint256_t &obj)
{
	for (int i = 0; i <  4; i++)
		this->parts[i] &= obj.parts[i];

	return (*this);
}


uint256_t	&uint256_t::operator|=(const uint256_t &obj)
{
	for (int i = 0; i <  4; i++)
		this->parts[i] |= obj.parts[i];

	return (*this);
}


uint256_t	&uint256_t::operator^=(const uint256_t &obj)
{
	for (int i = 0; i <  4; i++)
		this->parts[i] ^= obj.parts[i];

	return (*this);
}


uint256_t	&uint256_t::operator>>=(int shift)
{
	if (shift >= 256)
	{
		this->parts[3] = 0ull;
		this->parts[2] = 0ull;
		this->parts[1] = 0ull;
		this->parts[0] = 0ull;
		return (*this);
	}

	if (shift >= 192)
	{
		this->parts[3] = this->parts[0];
		this->parts[2] = 0ull;
		this->parts[1] = 0ull;
		this->parts[0] = 0ull;
		shift -= 192;
	}
	else if (shift >= 128)
	{
		this->parts[3] = this->parts[1];
		this->parts[2] = this->parts[0];
		this->parts[1] = 0ull;
		this->parts[0] = 0ull;
		shift -= 128;
	}
	else if (shift >= 64)
	{
		this->parts[3] = this->parts[2];
		this->parts[2] = this->parts[1];
		this->parts[1] = this->parts[0];
		this->parts[0] = 0ull;
		shift -= 64;
	}

	if (shift == 0)
		return (*this);

	int			invShift = 64 - shift;
	uint64_t	mask = createLengthMask(shift);

	// Part 3
	this->parts[3] = this->parts[3] >> shift;
	this->parts[3] |= (this->parts[2] & mask) << invShift;

	// Part 2
	this->parts[2] = this->parts[2] >> shift;
	this->parts[2] |= (this->parts[1] & mask) << invShift;

	// Part 1
	this->parts[1] = this->parts[1] >> shift;
	this->parts[1] |= (this->parts[0] & mask) << invShift;

	// Part 0
	this->parts[0] = this->parts[0] >> shift;

	return (*this);
}


uint256_t	&uint256_t::operator<<=(int shift)
{
	if (shift >= 256)
	{
		this->parts[0] = 0ull;
		this->parts[1] = 0ull;
		this->parts[2] = 0ull;
		this->parts[3] = 0ull;
		return (*this);
	}

	if (shift >= 192)
	{
		this->parts[0] = this->parts[3];
		this->parts[1] = 0ull;
		this->parts[2] = 0ull;
		this->parts[3] = 0ull;
		shift -= 192;
	}
	else if (shift >= 128)
	{
		this->parts[0] = this->parts[2];
		this->parts[1] = this->parts[3];
		this->parts[2] = 0ull;
		this->parts[3] = 0ull;
		shift -= 128;
	}
	else if (shift >= 64)
	{
		this->parts[0] = this->parts[1];
		this->parts[1] = this->parts[2];
		this->parts[2] = this->parts[3];
		this->parts[3] = 0ull;
		shift -= 64;
	}

	if (shift == 0)
		return (*this);

	int			invShift = 64 - shift;
	uint64_t	mask = createLengthMask(shift) << invShift;

	// Part 0
	this->parts[0] = this->parts[0] << shift;
	this->parts[0] |= (this->parts[1] & mask) >> invShift;

	// Part 1
	this->parts[1] = this->parts[1] << shift;
	this->parts[1] |= (this->parts[2] & mask) >> invShift;

	// Part 2
	this->parts[2] = this->parts[2] << shift;
	this->parts[2] |= (this->parts[3] & mask) >> invShift;

	// Part 3
	this->parts[3] = this->parts[3] << shift;

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

uint256_t	uint256_t::operator&(const uint256_t &obj) const
{
	uint256_t	result;

	for (int i = 0; i <  4; i++)
		result.parts[i] = this->parts[i] & obj.parts[i];

	return (result);
}


uint256_t	uint256_t::operator|(const uint256_t &obj) const
{
	uint256_t	result;

	for (int i = 0; i <  4; i++)
		result.parts[i] = this->parts[i] | obj.parts[i];

	return (result);
}


uint256_t	uint256_t::operator^(const uint256_t &obj) const
{
	uint256_t	result;

	for (int i = 0; i <  4; i++)
		result.parts[i] = this->parts[i] ^ obj.parts[i];

	return (result);
}


uint256_t	uint256_t::operator~(void) const
{
	uint256_t	result;

	for (int i = 0; i <  4; i++)
		result.parts[i] = ~this->parts[i];

	return (result);
}


uint256_t	uint256_t::operator>>(int shift) const
{
	uint256_t	result;

	if (shift >= 256)
		return (result);

	if (shift >= 192)
	{
		result.parts[3] = this->parts[0];
		shift -= 192;
	}
	else if (shift >= 128)
	{
		result.parts[2] = this->parts[0];
		result.parts[3] = this->parts[1];
		shift -= 128;
	}
	else if (shift >= 64)
	{
		result.parts[1] = this->parts[0];
		result.parts[2] = this->parts[1];
		result.parts[3] = this->parts[2];
		shift -= 64;
	}
	else
	{
		result = *this;
	}

	if (shift == 0)
		return (result);

	int			invShift = 64 - shift;
	uint64_t	mask = createLengthMask(shift);

	// Part 3
	result.parts[3] = result.parts[3] >> shift;
	result.parts[3] |= (result.parts[2] & mask) << invShift;

	// Part 2
	result.parts[2] = result.parts[2] >> shift;
	result.parts[2] |= (result.parts[1] & mask) << invShift;

	// Part 1
	result.parts[1] = result.parts[1] >> shift;
	result.parts[1] |= (result.parts[0] & mask) << invShift;

	// Part 0
	result.parts[0] = result.parts[0] >> shift;

	return (result);
}


uint256_t	uint256_t::operator<<(int shift) const
{
	uint256_t	result;

	if (shift >= 256)
		return (result);

	if (shift >= 192)
	{
		result.parts[0] = this->parts[3];
		shift -= 192;
	}
	else if (shift >= 128)
	{
		result.parts[0] = this->parts[2];
		result.parts[1] = this->parts[3];
		shift -= 128;
	}
	else if (shift >= 64)
	{
		result.parts[0] = this->parts[1];
		result.parts[1] = this->parts[2];
		result.parts[2] = this->parts[3];
		shift -= 64;
	}
	else
	{
		result = *this;
	}

	if (shift == 0)
		return (result);

	int			invShift = 64 - shift;
	uint64_t	mask = createLengthMask(shift) << invShift;

	// Part 0
	result.parts[0] = result.parts[0] << shift;
	result.parts[0] |= (result.parts[1] & mask) >> invShift;

	// Part 1
	result.parts[1] = result.parts[1] << shift;
	result.parts[1] |= (result.parts[2] & mask) >> invShift;

	// Part 2
	result.parts[2] = result.parts[2] << shift;
	result.parts[2] |= (result.parts[3] & mask) >> invShift;

	// Part 3
	result.parts[3] = result.parts[3] << shift;

	return (result);
}

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
