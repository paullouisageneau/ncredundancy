
#include "network.h"

#include <boost/random/uniform_on_sphere.hpp>
#include <boost/random/uniform_01.hpp>

namespace ncr
{

Network::Network(unsigned long seed) :
	threshold(1.),
	nextStepNode(0)
{
	gen.seed(seed);
}

Network::~Network(void)
{

}

void Network::setThreshold(double thresh)
{
	threshold = thresh;	
}
	
double Network::getThreshold(void)
{
	return threshold;
}

void Network::generateRandom(unsigned long seed, int count, double radius)
{
	boost::mt19937 gen;
	boost::uniform_on_sphere<double> uniform(2);
	
	nodes.clear();
	nodes.reserve(count);
	for(int i=0; i<count; ++i)
	{
    		std::vector<double> position = uniform(gen);
		nodes.push_back(Node(i, position[0]*radius, position[1]*radius));
	}
}

void Network::generateGrid(int nx, int ny, double stepx, double stepy)
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
	for(int i=0; i<count(); ++i)
	{
		std::cout << nodes[i] << std::endl;
	}
}

int Network::count(void) const
{
	return int(nodes.size());
}

double Network::linkQualityFromDistance(double distance)
{
	return 0.5;	// TODO
}

void Network::update(void)
{
	computeLinkMatrix(links);
	computeAdjacencyMatrix(adjacency);
	computeRouting(nexthops, distances);
}

void Network::send(int source, int destination, unsigned count)
{
	std::vector<int> nexthops;
	getNextHops(source, source, destination, nexthops);
	
	for(unsigned c=0; c<count; ++c)
	{
		Packet packet(destination, 1024, c);
		
		if(nodes[source].recv(packet))
		{
			nodes[source].relay(packet, nexthops);
		}
	}
}

bool Network::step(void)
{
	int first = nextStepNode;
	for(int i=0; i<count(); i++)
	{
		int node = (first + i) % count();
		nextStepNode = (node + 1) % count();
		
		Packet packet;
		if(nodes[node].send(packet))
		{
			sendPacket(packet, node);
			return true;
		}
	}
	
	return false;
}

unsigned Network::received(int i) const
{
	return nodes[i].received();
}

double Network::linkQuality(int i, int j)
{
	return links(i,j);
}

bool Network::areNeighbors(int i, int j)
{
	return adjacency(i,j);
}

void Network::getNeighbors(int i, std::vector<int> &result)
{
	result.clear();
	
	for(int j=0; j<count(); j++)
	{
		if(areNeighbors(i,j)) result.push_back(j); // note: when i==j, they are not neighbors	
	}
}

void Network::getLinkQuality(int i, std::vector<double> &result)
{
	result.clear();
	
	std::vector<int> neighbors;
	getNeighbors(i, neighbors);
	
	for(std::vector<int>::iterator it = neighbors.begin(); it != neighbors.end(); it++)
	{
		result.push_back(linkQuality(i,*it));
	}
}

void Network::sendPacket(const Packet &packet, int sender)
{
	std::vector<int> neighbors;
	getNeighbors(sender, neighbors);
	
	for(int i=0; i<int(neighbors.size()); ++i)
	{
		int v = neighbors[i];
		double q = linkQuality(sender, v);
		
		boost::uniform_01<double> uniform;
		double p = uniform(gen);
		
		if(p < q)
		{
			if(nodes[v].recv(packet))
			{
				std::vector<int> nexthops;
				getNextHops(v, sender, packet.destination, nexthops);
				nodes[v].relay(packet, nexthops);
			}
		}
	}
}

void Network::getNextHops(int i, int from, int to, std::vector<int> &result)
{
	getNeighbors(i, result);
	
	// TODO: model
	
}

void Network::computeAdjacencyMatrix(matrix<bool> &result)
{
	result.resize(count(), count());
	
	int i,j;
	for(i=0; i<count(); i++)
	{
		for(j=0; j<count(); j++)
		{
			if(i==j) result(i,j)=false;   // define: a node is NOT a neighbor to itself
			else result(i,j)=(nodes[i].distance(nodes[j]) <= threshold);
		}
	}
}

void Network::computeLinkMatrix(matrix<double> &result)
{
	result.resize(count(), count());
	
	int i,j;
	for(i=0; i<count(); i++)
	{
		for(j=0; j<count(); j++)
		{
			if(i==j) result(i,j)=1;   // define: q_ii = 1
			else result(i,j)=linkQualityFromDistance(nodes[i].distance(nodes[j]));
		}
	}
}

void Network::computeRouting(matrix<int> &nexthops, matrix<int> &distances)
{
	std::vector<bool> visited;
	std::vector<int>  dist;
	std::vector<int>  prev;
	
	// Compute paths for each node as source
	for(int s=0; s<count(); ++s)
	{
		visited.assign(count(), false);		// all nodes unvisited at first
		dist.assign(count(), count());		// all distances infinite at first
		prev.assign(count(), -1);		// all previous nodes undefined at first
	
		int c = s;	// current node is source node
		dist[c] = 0;	// source distance is zero
		
		while(c >= 0)
		{
			// Current node is visited
			visited[c] = true;
			
			// Iterate on neighbors
			std::vector<int> neighbors;
			getNeighbors(c, neighbors);
			for(int i=0; i<int(neighbors.size()); ++i)
			{
				int v = neighbors[i];
				int w = 1;		// neighbors are at distance 1
				if(dist[c]+w < dist[v])
				{
					dist[v] = dist[c]+w;
					prev[v] = c;
				}
			}
			
			c = -1;
			for(int i=0; i<count(); ++i)
				if(!visited[i] && (c < 0 || dist[i] < dist[c]))
					c = i;
		}
		
		// Fill matrixes
		nexthops.resize(count(), count());
		distances.resize(count(), count());
		
		for(int i=0; i<count(); ++i)
		{
			// Find next hop
			int next = i;
			while(prev[next] != s)
			{
				next = prev[next];
				if(prev[next] == -1)
					break;
			}
			
			nexthops(s, i)  = next;
			distances(s, i) = dist[i];
		}
	}
}

}

