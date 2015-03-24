
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

void Network::generateRandom(unsigned long seed, int count, double radius)
{
	boost::mt19937 gen;
	boost::uniform_on_sphere<double> uniform(2);
	gen.seed(seed);

	nodes.clear();
	nodes.reserve(count);
	for(int i=0; i<count; ++i)
	{
    		std::vector<double> position = uniform(gen);
		nodes.push_back(Node(position[0]*radius, position[1]*radius));
	}
}

void Network::generateMesh(int nx, int ny, double stepx, double stepy)
{
	nodes.clear();
	nodes.reserve(nx*ny);
	for(int i=0; i<nx; ++i)
		for(int j=0; j<ny; ++j)
			nodes.push_back(Node(i*stepx, j*stepy));
}

void Network::print(void) const
{
	for(int i=0; i<nodes.size(); ++i)
	{
		std::cout << "Node " << i << ": (x,y) = (" << nodes[i].x << "," << nodes[i].y << ")"<< std::endl;
	}
}

unsigned int Network::count(void) const
{
	return nodes.size();
}

double Network::linkQualityFromDistance(double distance)
{
	// TODO
	double reference = 1.;
	return std::min(1., reference/(distance*distance));
}

double Network::linkQuality(int i, int j)
{
	return linkQualityFromDistance(nodes[i].distance(nodes[j]));
}

void Network::setThreshold(double thresh)
{
	threshold = thresh;	
}
	
double Network::getThreshold ()
{
	return threshold;
}

bool Network::areNeighbors(int i, int j)
{
	return adjancency(i,j);
}

void Network::computeAdjacencyMatrix(matrix<bool> &result)
{
	int i,j;
	for(i=0; i<nodes.size(); i++)
	{
		for(j=0; j<nodes.size(); j++)
		{
			if(i==j) result(i,j)=false;   // define: a node is NOT a neighbor to itself
			else result(i,j)=(nodes[i].distance(nodes[j]) <= threshold);
		}
	}
}


}

