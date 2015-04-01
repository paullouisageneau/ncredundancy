
#include "node.h"

namespace ncr
{

Node::Node(int _id, double _x, double _y) :
	id(_id),
	x(_x),
	y(_y),
	accumulator(0.),
	forward(false)
{

}

Node::~Node(void)
{

}

void Node::recv(const Packet &packet, int from)
{
	if(forward)
	{
		if(packet.destination != id)
			outgoing.push(packet);
	}
	else {
		if(!rlc.solve(packet))
			return;	// Not innovative
		
		if(packet.destination == id)
			return; // We are the destination
		
		if(from >= 0 && from != id && pathExists(from, packet.destination, distances[packet.destination]-1))
			return;	// We are not a next hop
			
		std::vector<int> nexthops;
		getNextHops(packet.destination, nexthops); 
		
		double sigma = 0;
		double p = 1.;
		for(int i=0; i<int(nexthops.size()); ++i)
		{
			sigma+= links[i];
			p*= 1 - links[i];
		}
		
		const int m = rlc.componentsCount();
		const double tau = 0.01;
		
		const double C = (-std::log(tau)/m) * (p/(1-p));
		const double rbound = 1/(1-p) * (1 + std::sqrt(2*C));
		
		double redundancy = rbound/sigma;
		
		accumulator+= redundancy;
		while(accumulator >= 1.)
		{
			accumulator-= 1.;
		
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

bool Node::pathExists(int i, int j, int distance)
{
	if(distance <= 0)
		return (i == j);
	
	matrix<bool> a = adjacency;
	while(--distance)
		a = prod(a,adjacency);
	
	return a(i,j);
}

void Node::getNextHops(int j, std::vector<int> &nexthops)
{
	nexthops.clear();
	for(int i=0; i<int(neighbors.size()); ++i)
	{
		int v = neighbors[i];
		if(pathExists(v, j, distances[j]))
			nexthops.push_back(v);
	}
}

std::ostream &operator<<(std::ostream &s, const Node &node)
{
	s << "node " << node.id << " (" << node.x << "," << node.y << "): decoded=" << node.rlc.decodedCount() << ", outgoing=" << node.outgoing.size();
	return s;
}

}

