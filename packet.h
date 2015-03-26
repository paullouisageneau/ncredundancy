
#ifndef NCR_PACKET_H
#define NCR_PACKET_H

#include "main.h"
#include "rlc.h"

#include <map>

namespace ncr
{

class Packet
{
public:
	Packet(int _size, int _index = -1);
	~Packet(void);

	Rlc::Combination combination;	// Coefficients
	int size;			// Data size
};

}

#endif

