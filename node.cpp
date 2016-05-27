
#include "node.h"
#include "network.h" // TODO: remove

namespace ncr
{

int Node::PacketSize = 1024;
int Node::GenerationSize = 16;
double Node::Tau = 0.01;
	
Node::Node(int _id, double _x, double _y) :
	id(_id),
	x(_x),
	y(_y),
	alpha(1.),
	accumulator(0.),
	forward(false),
	jamming(0.),
	emitted(0)
{

}

Node::~Node(void)
{

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

void Node::generate(int destination, unsigned count)
{
	if(forward)
	{
		for(unsigned c=0; c<count; ++c)
		{
			Packet packet(id, destination, PacketSize, c);
			outgoing.push(packet);
		}
	}
	else {
		unsigned offset = rlc.componentsCount();
		
		// Batch
		rlc.fill(offset + count);
		rlcRelay(id, id, destination, count);
		flush(id, destination);
		
		// Pipeline
		//for(unsigned c=0; c<count; ++c)
		//{
		//	Packet packet(id, destination, PacketSize, offset + c);
		//	recv(packet, id);
		//}
	}
}

void Node::recv(const Packet &packet, int from)
{
	if(from < 0)
		from = id;
	
	if(forward)
	{
		uint64_t uid = packet.uid();
		if(packet.destination != id && forwarded.find(uid) == forwarded.end())
		{
			forwarded.insert(uid);
			outgoing.push(packet);
			++emitted;
		}
	}
	else {
		if(!rlc.solve(packet))
			return;	// Not innovative
		
		if(packet.destination == id)
			return; // We are the destination
		
		if(from != id && pathExists(from, packet.destination, distances[packet.destination]-1))
			return;	// We are not a next hop
		
		rlcRelay(from, packet.source, packet.destination, 1);
		//flush(packet.source, packet.destination);	// on-the-fly recoding
	}
}

void Node::flush(int source, int destination)
{
	while(accumulator >= 1.)
	{
		accumulator-= 1.;
		
		Packet out(source, destination, PacketSize);	// create
		rlc.generate(out);				// generate
		outgoing.push(out);				// push out
		++emitted;
	}
}

bool Node::send(Packet &packet)
{
	if(outgoing.empty())
		return false;
	
	packet = outgoing.front();
	outgoing.pop();
	if(outgoing.empty())
		packet.last = true;
	return true;
}

unsigned Node::received(void) const
{
	return rlc.decodedCount();
}

unsigned Node::seen(void) const
{
	return rlc.seenCount();
}

void Node::reset(void)
{
	rlc.clear();
	//accumulator = 0.;
	
	while(!outgoing.empty())
		outgoing.pop();
	
	forwarded.clear();
	emitted = 0;
}

bool Node::pathExists(int i, int j, int distance) const
{
	if(distance <= 0)
		return (i == j);
	
	matrix<bool> a = adjacency;
	while(--distance)
		a = prod(a,adjacency);
	
	return a(i,j);
}

void Node::getNextHops(int i, int j, std::vector<int> &nexthops) const
{
	nexthops.clear();
	
	if(i == j) 
		return;
	
	for(int v=0; v<int(adjacency.size2()); ++v)
	{
		if(v == i)
			continue;
		
		if(adjacency(i, v))
		{
			if(pathExists(v, j, distances[i] + distances[j] - 1))
			{
				nexthops.push_back(v);
			}
		}
	}
}

void Node::rlcRelay(int from, int source, int destination, unsigned count)
{
	if(from < 0)
		from = id;
	
	double sigma = 1.;
	if(from != id)
	{
		sigma = 0.;
		std::vector<int> nexthops;
		getNextHops(from, destination, nexthops);
		for(int i=0; i<int(nexthops.size()); ++i)
		{
			int n = nexthops[i];
			sigma+= links(from,n)*alphas[n];
		}
	}

	double p = 1.;
	std::vector<int> nexthops;
	getNextHops(id, destination, nexthops);
	if(nexthops.empty())
		return;
	
	for(int i=0; i<int(nexthops.size()); ++i)
	{
		int n = nexthops[i];
		p*= 1. - links(id,n)*alphas[n];
	}

	p+= 0.004*2;	// should be ~ 0.004
	
	const int m = GenerationSize;
	const int d = distances[source] + distances[destination];
	const double tau = 1. - std::pow(1.-Tau, 1./d);
	const double C = (-std::log(tau)/m) * (p/(1.-p));
	const double A = (1. + std::sqrt(2.*C));
	const double rbound = 1./(1.-p) * (1. + A*A)/2.;
	const double redundancy = rbound*alpha/sigma;
	
	//std::cout << "node " << id << "\tnexthops=" << nexthops.size() << "\tp=" << p << "\ttau_local=" << tau << "\tr_avg=" << 1./(1.-p) << "\tr_bound=" << rbound << "\tsigma=" << sigma << "\talpha=" << alpha << "\tredundancy=" << redundancy << std::endl;

	// Test only
	//double redundancy = 1 + Node::Tau;

	accumulator+= redundancy*count;
}

std::ostream &operator<<(std::ostream &s, const Node &node)
{
	s << "node " << node.id << " (" << node.x << "," << node.y << "): seen=" << node.rlc.seenCount() << ", decoded=" << node.rlc.decodedCount() << ", outgoing=" << node.outgoing.size();
	return s;
}

}

