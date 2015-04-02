
#include "rlc.h"

#include <boost/random/uniform_int_distribution.hpp>

namespace ncr
{

uint8_t *Rlc::MulTable = NULL;
uint8_t *Rlc::InvTable = NULL;
boost::random::mt19937 Rlc::Gen;

void Rlc::Init(unsigned long seed)
{
	Gen.seed(seed); // Seed generator
	
	if(!MulTable) 
	{
		MulTable = new uint8_t[256*256];
		
		MulTable[0] = 0;
		for(uint8_t i = 1; i != 0; ++i) 
		{
			MulTable[unsigned(i)] = 0;
			MulTable[unsigned(i)*256] = 0;
			
			for(uint8_t j = 1; j != 0; ++j)
			{
				uint8_t a = i;
				uint8_t b = j;
				uint8_t p = 0;
				uint8_t k;
				uint8_t carry;
				for(k = 0; k < 8; ++k)
				{
					if (b & 1) p^= a;
					carry = (a & 0x80);
					a<<= 1;
					if (carry) a^= 0x1b; // 0x1b is x^8 modulo x^8 + x^4 + x^3 + x + 1
					b>>= 1;
				}
				
				MulTable[unsigned(i)*256+unsigned(j)] = p;
			}
		}
	}
	
	if(!InvTable)
	{
		InvTable = new uint8_t[256];
		
		InvTable[0] = 0;
		for(uint8_t i = 1; i != 0; ++i)
		{
			for(uint8_t j = i; j != 0; ++j)
			{
				if(Rlc::gMul(i,j) == 1)	// then Rlc::gMul(j,i) == 1
				{
					InvTable[i] = j;
					InvTable[j] = i;
				}
			}
		}
	}
}

void Rlc::Cleanup(void)
{
	delete[] MulTable;
	delete[] InvTable;
	MulTable = NULL;
	InvTable = NULL;
}

uint8_t Rlc::gAdd(uint8_t a, uint8_t b)
{
	return a ^ b;
}

uint8_t Rlc::gMul(uint8_t a, uint8_t b) 
{
	return MulTable[unsigned(a)*256+unsigned(b)];
}

uint8_t Rlc::gInv(uint8_t a) 
{
	return InvTable[a];
}

Rlc::Combination::Combination(void)
{
	
}

Rlc::Combination::Combination(const Combination &combination)
{
	*this = combination;
}

Rlc::Combination::~Combination(void)
{
	
}

void Rlc::Combination::addComponent(unsigned offset, uint8_t coeff)
{
	std::map<unsigned, uint8_t>::iterator it = mComponents.find(offset);
	if(it != mComponents.end())
	{
		it->second = Rlc::gAdd(it->second, coeff);
		if(it->second == 0)
			mComponents.erase(it);
	}
	else {
		if(coeff != 0)
			mComponents[offset] = coeff;
	}
}

unsigned Rlc::Combination::firstComponent(void) const
{
	if(!mComponents.empty()) return mComponents.begin()->first;
	else return 0;
}

unsigned Rlc::Combination::lastComponent(void) const
{
	if(!mComponents.empty()) return (--mComponents.end())->first;
	else return 0;
}

unsigned Rlc::Combination::componentsCount(void) const
{
	if(!mComponents.empty()) return (lastComponent() - firstComponent()) + 1;
	else return 0;
}

uint8_t Rlc::Combination::coeff(unsigned offset) const
{
	std::map<unsigned, uint8_t>::const_iterator it = mComponents.find(offset);
	if(it == mComponents.end()) return 0; 
	return it->second;
}

bool Rlc::Combination::isCoded(void) const
{
	return (mComponents.size() != 1 || mComponents.begin()->second != 1);
}

bool Rlc::Combination::isNull(void) const
{
	return (mComponents.size() == 0);
}

void Rlc::Combination::clear(void)
{
	mComponents.clear();
}

Rlc::Combination &Rlc::Combination::operator=(const Combination &combination)
{
	mComponents = combination.mComponents;
	return *this;
}

Rlc::Combination Rlc::Combination::operator+(const Combination &combination) const
{
	Rlc::Combination result(*this);
	result+= combination;
	return result;
}

Rlc::Combination Rlc::Combination::operator*(uint8_t coeff) const
{
	Rlc::Combination result(*this);
	result*= coeff;
	return result;
}

Rlc::Combination Rlc::Combination::operator/(uint8_t coeff) const
{
	Rlc::Combination result(*this);
	result/= coeff;	
	return result;
}
	
Rlc::Combination &Rlc::Combination::operator+=(const Combination &combination)
{
	// Add components
	for(	std::map<unsigned, uint8_t>::const_iterator jt = combination.mComponents.begin();
		jt != combination.mComponents.end();
		++jt)
	{
		addComponent(jt->first, jt->second);
	}
	
	return *this;
}

Rlc::Combination &Rlc::Combination::operator*=(uint8_t coeff)
{
	if(coeff != 1)
	{
		if(coeff != 0)
		{
			for(	std::map<unsigned, uint8_t>::iterator it = mComponents.begin();
				it != mComponents.end();
				++it)
			{
				it->second = Rlc::gMul(it->second, coeff);
			}
		}
		else {
			mComponents.clear();
		}
	}
	
	return *this;
}

Rlc::Combination &Rlc::Combination::operator/=(uint8_t coeff)
{
	assert(coeff != 0);

	(*this)*= Rlc::gInv(coeff);
	return *this;
}

Rlc::Rlc(void) :
	mDecodedCount(0),
	mComponentsCount(0)
{
	
}

Rlc::~Rlc(void)
{
	
}

void Rlc::fill(unsigned count)
{
	clear();
	
	for(unsigned i = 0; i<count; ++i)
		mCombinations[i].addComponent(i, 1);
	
	mComponentsCount = count;
}

void Rlc::clear(void)
{
	mCombinations.clear();
	mDecodedCount = 0;
	mComponentsCount = 0;
}

bool Rlc::generate(Combination &output) const
{	
	output.clear();
	
	if(mCombinations.empty())
		return false;
	
	boost::random::uniform_int_distribution<> dist(1, 256);		// no zero
	for(std::map<unsigned, Combination>::const_iterator it = mCombinations.begin();
		it != mCombinations.end();
		++it)
	{
		uint8_t coeff = uint8_t(dist(Gen));
		output+= it->second*coeff;
	}
	
	return true;
}

bool Rlc::solve(Combination incoming)
{
	if(incoming.isNull())
		return false;
	
	mComponentsCount = std::max(mComponentsCount, incoming.lastComponent()+1);
	
	// ==== Gauss-Jordan elimination ====
	
	std::map<unsigned, Combination>::iterator it, jt;
	std::map<unsigned, Combination>::reverse_iterator rit;
	
	// Eliminate coordinates, so the system is triangular
	for(unsigned i = incoming.firstComponent(); i <= incoming.lastComponent(); ++i)
	{
		uint8_t c = incoming.coeff(i);
		if(c != 0)
		{
			jt = mCombinations.find(i);
			if(jt == mCombinations.end()) break;
			incoming+= jt->second*c;
		}
	}
	
	if(incoming.isNull())
		return false;	// non-innovative combination
	
	// Insert incoming combination
	incoming/= incoming.coeff(incoming.firstComponent());
	mCombinations[incoming.firstComponent()] = incoming;
	
	// Attempt to substitute to solve
	rit = mCombinations.rbegin();
	while(rit != mCombinations.rend())
	{
		unsigned first = std::max(rit->second.firstComponent(), rit->first);
		for(unsigned i = rit->second.lastComponent(); i > first; --i)
		{
			jt = mCombinations.find(i);
			if(jt != mCombinations.end())
			{
				if(jt->second.isCoded()) break;
				rit->second+= jt->second*rit->second.coeff(i);
			}
		}

		if(rit->second.lastComponent() != rit->first)
			break;
		
		++rit;
	}
	
	// Remove null components and count decoded
	mDecodedCount = 0;
	it = mCombinations.begin();
	while(it != mCombinations.end())
	{
		if(it->second.isNull())
		{
			// Null vector, useless equation
			mCombinations.erase(it++);
		}
		else {
			if(!it->second.isCoded())
				++mDecodedCount;
			
			++it;
		}
	}
	
	return true;	// incoming was innovative
}

void Rlc::print(void) const
{
	for(std::map<unsigned, Combination>::const_iterator it = mCombinations.begin();
		it != mCombinations.end();
		++it)
	{
		std::cout << it->second << std::endl;
	}
}

unsigned Rlc::seenCount(void) const
{
	return mCombinations.size();
}

unsigned Rlc::decodedCount(void) const
{
	return mDecodedCount;
}

unsigned Rlc::componentsCount(void) const
{
	return mComponentsCount;
}

std::ostream &operator<<(std::ostream &s, const Rlc::Combination &c)
{
	s << "combination (";
	if(!c.isNull())
		for(unsigned i = 0; i <= c.lastComponent(); ++i)
			s << (i ? ", " : "") << unsigned(c.coeff(i));
	s << ")";
	return s;
}

}
