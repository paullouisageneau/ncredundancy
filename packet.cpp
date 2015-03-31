
#include "packet.h"

namespace ncr
{

Packet::Packet(int _destination, int _size, unsigned _index) :
	destination(_destination),
	size(_size)
{
	if(_index >= 0)
		addComponent(_index, 1);
}

Packet::~Packet(void)
{

}

}

