
#ifndef NCR_PACKET_H
#define NCR_PACKET_H

#include "main.h"

#include <map>

namespace ncr
{

class Packet
{
public:
	Packet(int _size, int _index = -1);
	~Packet(void);

	int size;			// data size
	std::map<int, uint8_t> coeffs;	// coefficients
};

}

#endif

