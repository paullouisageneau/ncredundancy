
#include "packet.h"

namespace ncr
{

Packet::Packet(int _source, int _destination, int _size, unsigned _index) :
	source(_source),
	destination(_destination),
	size(_size),
	last(false)
{
	if(_index >= 0)
		addComponent(_index, 1);
}

Packet::~Packet(void)
{

}

}

