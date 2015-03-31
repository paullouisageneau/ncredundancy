
#include "node.h"

namespace ncr
{

Node::Node(int _id, double _x, double _y) :
	id(_id),
	x(_x),
	y(_y),
	forward(false)
{

}

Node::~Node(void)
{

}

bool Node::recv(const Packet &packet)
{
	if(forward)
	{
		outgoing.push(packet);
		return (packet.destination != id);
	}
	else {
		bool isInnovative = rlc.solve(packet);
		return (isInnovative && packet.destination != id);
	}
}

void Node::relay(const Packet &packet, const std::vector<int> &nexthops)
{
	if(!forward && packet.destination != id)
	{
		// TODO: model, compute redundancy using links and apply it
		
		Packet out(packet.destination, packet.size);	// create
		rlc.generate(out);				// generate
		outgoing.push(out);				// push out
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

unsigned Node::received(void) const
{
	return rlc.decodedCount();
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

std::ostream &operator<<(std::ostream &s, const Node &node)
{
	s << "node " << node.id << " (" << node.x << "," << node.y << "): decoded=" << node.rlc.decodedCount() << ", outgoing=" << node.outgoing.size();
	return s;
}

}

