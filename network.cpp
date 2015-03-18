
#include "network.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_on_sphere.hpp>

namespace ncr
{

Network::Network(void)
{

}

Network::~Network(void)
{

}

unsigned int Network::count(void) const
{
	return nodes.size();
}

void Network::generate(unsigned long seed, unsigned int count, double radius)
{
	boost::mt19937 gen;
	boost::uniform_on_sphere<double> uniform(2);
	gen.seed(seed);

	nodes.clear();
	nodes.reserve(count);
	for(unsigned int i=0; i<count; ++i)
	{
    		std::vector<double> position = uniform(gen);
		nodes.push_back(Node(position[0]*radius, position[1]*radius));
	}
}

void Network::print(void) const
{
	for(unsigned int i=0; i<nodes.size(); ++i)
	{
		std::cout << "Node " << i << ": (x,y) = (" << nodes[i].x << "," << nodes[i].y << ")"<< std::endl;
	}
}

}

