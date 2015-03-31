
#ifndef NCR_PACKET_H
#define NCR_PACKET_H

#include "main.h"
#include "rlc.h"

#include <map>

namespace ncr
{

class Packet : public Rlc::Combination
{
public:
	Packet(int _destination = -1, int _size = 0, unsigned _index = -1);
	~Packet(void);

	int destination;	// Destination node
	int size;		// Data size
};

}

#endif

