
#ifndef NCR_RLC_H
#define NCR_RLC_H

#include "main.h"

#include <map>
#include <boost/random/mersenne_twister.hpp>

namespace ncr
{

// Fake random linear coding implementation
class Rlc
{
public:
	static void Init(unsigned long seed);
	static void Cleanup(void);
	
	class Combination
	{
	public:
		Combination(void);
		Combination(const Combination &combination);
		Combination(unsigned offset);
		~Combination(void);
		
		void addComponent(unsigned offset, uint8_t coeff);
		unsigned firstComponent(void) const;
		unsigned lastComponent(void) const;
		unsigned componentsCount(void) const;
		uint8_t coeff(unsigned offset) const;
		bool isCoded(void) const;
		bool isNull(void) const;
		void clear(void);
		
		uint64_t uid(void) const;
		
		Combination &operator=(const Combination &combination);
		Combination operator+(const Combination &combination) const;
		Combination operator*(uint8_t coeff) const;
		Combination operator/(uint8_t coeff) const;
		Combination &operator+=(const Combination &combination);
		Combination &operator*=(uint8_t coeff);
		Combination &operator/=(uint8_t coeff);
		
	private:
		std::map<unsigned, uint8_t> mComponents;
	};
	
	Rlc(void);
	~Rlc(void);
	
	unsigned size(void) { return seenCount(); }
	void fill(unsigned count);	// Fill with count components (source)
	void clear(void);		// Clear system
	
	bool generate(Combination &output) const;	// Generate combination with count components
	bool solve(Combination incoming);		// Add combination and try to solve, return true if innovative
	void print(void) const;				// Print current system
	
	unsigned seenCount(void) const;			// Return seen combinations count (degree)
	unsigned decodedCount(void) const;		// Return decoded combinations count
	unsigned componentsCount(void) const;		// Return number of components in system
	
private:
	// GF(2^8) operations
	static uint8_t gAdd(uint8_t a, uint8_t b);
	static uint8_t gMul(uint8_t a, uint8_t b); 
	static uint8_t gInv(uint8_t a);
	
	// GF(2^8) operations tables
	static uint8_t *MulTable;
	static uint8_t *InvTable;
	
	// Pseudo-random generator
	static boost::random::mt19937 Gen;
	
	std::map<unsigned, Combination> mCombinations;	// combinations sorted by pivot component
	unsigned mDecodedCount;
	unsigned mComponentsCount;
};

std::ostream &operator<< (std::ostream &s, const Rlc::Combination &c);

}

#endif
