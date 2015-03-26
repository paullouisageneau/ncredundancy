
#include "packet.h"

namespace ncr
{

Packet::Packet(int _size, int _index) :
	size(_size)
{
	if(_index >= 0)
		addComponent(unsigned(_index), 1);
}

Packet::~Packet(void)
{

}

}

