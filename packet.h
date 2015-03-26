
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
	Packet(int _destination, int _size, int _index = -1);
	~Packet(void);

	int destination;	// Destination node
	int size;		// Data size
};

}

#endif

