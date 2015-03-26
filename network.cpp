
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

void Network::setThreshold(double thresh)
{
	threshold = thresh;	
}
	
double Network::getThreshold ()
{
	return threshold;
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
		nodes.push_back(Node(i, position[0]*radius, position[1]*radius));
	}
}

void Network::generateMesh(int nx, int ny, double stepx, double stepy)
{
	nodes.clear();
	nodes.reserve(nx*ny);
	int i=0;
	for(int x=0; x<nx; ++x)
		for(int y=0; y<ny; ++y)
			nodes.push_back(Node(i++, x*stepx, y*stepy));
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
	return links(i,j);
}

bool Network::areNeighbors(int i, int j)
{
	return adjancency(i,j);
}

void Network::getNeighbors(int i, std::vector<int> &result)
{
	for(int j=0; j<nodes.size(); j++)
	{
		if(areNeighbors(i,j)) result.push_back(j); // note: when i==j, they are not neighbors	
	}
}

void Network::getLinkQuality(int i, std::vector<double> &result)
{
	std::vector<int> neighbors;
	getNeighbors(i, neighbors);
	for(std::vector<int>::iterator it = neighbors.begin(); it != neighbors.end(); it++)
	{
		result.push_back(linkQuality(i,*it));
	}
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

void Network::computeLinkMatrix(matrix<double> &result)
{
	int i,j;
	for(i=0; i<nodes.size(); i++)
	{
		for(j=0; j<nodes.size(); j++)
		{
			if(i==j) result(i,j)=1;   // define: q_ii = 1
			else result(i,j)=linkQualityFromDistance(nodes[i].distance(nodes[j]));
		}
	}
}

}

