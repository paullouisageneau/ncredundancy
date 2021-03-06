
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
	Packet(int source = -1, int _destination = -1, int _size = 0, unsigned _index = -1);
	~Packet(void);

	int source;		// Source node
	int destination;	// Destination node
	int size;		// Data size
	bool last;		// Last packet flag
};

}

#endif

