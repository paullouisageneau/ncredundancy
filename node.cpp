
#include "node.h"

namespace ncr
{

Node::Node(double _x, double _y) :
	x(_x),
	y(_y),
	forward(false)
{

}

Node::~Node(void)
{

}

void Node::recv(const Packet &packet)
{
	// TODO: apply redundancy
	
	if(forward)
	{
		outgoing.push(packet);
	}
	else {
		if(rlc.solve(packet))
		{
			// OK, packet is innovative
			Packet out(packet.destination, packet.size);	// create
			rlc.generate(out);				// generate
			outgoing.push(out);				// push out
		}
	}
}

bool Node::send(Packet &packet)
{
	if(outgoing.empty())
		return false;
	
	packet = outgoing.front();
	outgoing.pop();
	return true;
}

double Node::distance(const Node &node) const
{
	return std::sqrt(distance2(node));
}

double Node::distance2(const Node &node) const
{
	const double dx = x-node.x;
	const double dy = y-node.y;
	return dx*dx + dy*dy;
}

}

