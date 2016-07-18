
#include "network.h"

#include <boost/random/uniform_on_sphere.hpp>
#include <boost/random/uniform_01.hpp>

namespace ncr
{

Network::Network(unsigned long seed) :
	totalSent(0),
	totalReceived(0),
	totalLost(0),
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

//
//     o-o-o
//    /| | |\_
// (0)-o-o-o-(nx*ny+1)
//    \| | |/
//     o-o-o
//
void Network::generateGridoid(int nx, int ny, double stepx, double stepy)
{
	nodes.clear();
	nodes.reserve(nx*ny+2);
	int i=0;
	nodes.push_back(Node(i++, -stepx, stepy*(ny-1)*0.5));
	for(int x=0; x<nx; ++x)
		for(int y=0; y<ny; ++y)
			nodes.push_back(Node(i++, stepx*x, stepy*y));
	nodes.push_back(Node(i++, stepx*nx, stepy*(ny-1)*0.5));
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
	return 1.;	// TODO
}

void Network::update(void)
{
	updateFactors();
	
	computeLinkMatrix(links);
	computeAdjacencyMatrix(adjacency);
	
	routes.resize(count(), count());
	distances.resize(count(), count());

	for(int i=0; i<count(); ++i)
	{
		nodes[i].adjacency = adjacency;
		getNeighbors(i, nodes[i].neighbors);
		computeRouting(i, nodes[i].routes, nodes[i].distances);
		
		// Fill matrixes
		for(int j=0; j<count(); ++j)
		{
			routes(i, j)  = nodes[i].routes[j];
			distances(i, j) = nodes[i].distances[j];
		}
	}
}

void Network::updateFactors(void)
{
	// Fill link qualities and alphas
	for(int i=0; i<count(); ++i)
	{
		nodes[i].links = links;
		
		nodes[i].alphas.resize(count());
		for(int j=0; j<count(); ++j)
		{
			nodes[i].alphas[j] = nodes[j].alpha;
		}
	}
}

void Network::send(int source, int destination, unsigned count)
{
	nodes[source].generate(destination, count);
}

bool Network::step(bool flush)
{
	int first = nextStepNode;
	for(int i=0; i<count(); i++)
	{
		int n = (first + i) % count();
		nextStepNode = (n + 1) % count();
		
		if(flush) nodes[n].flush();
		
		Packet packet;
		if(nodes[n].send(packet))
		{
			sendPacket(packet, n);
			return true;
		}
	}
	
	if(!flush) return step(true);
	else return false;
}

void Network::reset(void)
{
	for(int i=0; i<count(); i++)
		nodes[i].reset();
	
	nextStepNode = 0;
}

unsigned Network::received(int i) const
{
	return nodes[i].received();
}

unsigned Network::seen(int i) const
{
	return nodes[i].seen();
}

unsigned Network::emitted(int i) const
{
	return nodes[i].emitted;
}

void Network::setForwarding(int i, bool enabled)
{
	nodes[i].forward = enabled;
}

bool Network::forwarding(int i) const
{
	return nodes[i].forward;
}

void Network::setJamming(int i, double jamming)
{
	nodes[i].jamming = jamming;
}

bool Network::jamming(int i) const
{
	return nodes[i].jamming;
}

void Network::setAlpha(int i, double alpha)
{
	nodes[i].alpha = alpha;
}

double Network::alpha(int i)
{
	return nodes[i].alpha;
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
		if(areNeighbors(i,j)) 
			result.push_back(j); // note: when i==j, they are not neighbors	
	}
}

void Network::getLinkQuality(int i, std::vector<double> &result)
{
	result.assign(count(), 0.);
	
	std::vector<int> neighbors;
	getNeighbors(i, neighbors);
	for(int j=0; j<int(neighbors.size()); ++j)
	{
		int v = neighbors[j];
		result[v] = linkQuality(i, v);
	}
}

void Network::sendPacket(const Packet &packet, int sender)
{
	++totalSent;
	
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
			// Transmitted
			nodes[v].recv(packet, sender);
			++totalReceived;
		}
		else {
			// Lost
			++totalLost;
		}
		
		//if(packet.last)
		//	nodes[v].flush();
	}
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
			else result(i,j) = linkQualityFromDistance(nodes[i].distance(nodes[j]))*(1.-nodes[j].jamming);
		}
	}
}

void Network::computeRouting(int s, std::vector<int> &routes, std::vector<int> &distances)
{
	std::vector<int>  prev;
	std::vector<bool> visited;
	
	distances.assign(count(), count());	// all distances infinite at first
	prev.assign(count(), -1);		// all previous nodes undefined at first
	visited.assign(count(), false);		// all nodes unvisited at first
	
	int c = s;		// current node is source node
	distances[c] = 0;	// source distance is zero
	
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
			if(distances[c]+w < distances[v])
			{
				distances[v] = distances[c]+w;
				prev[v] = c;
			}
		}
		
		c = -1;
		for(int i=0; i<count(); ++i)
			if(!visited[i] && (c < 0 || distances[i] < distances[c]))
				c = i;
	}
	
	// Fill routes
	routes.resize(count());
	routes[s] = s;
	for(int i=0; i<count(); ++i)
	{	
		if(i == s) continue;
		
		// Find next hop
		int next = i;
		while(prev[next] != s)
		{
			next = prev[next];
			if(next == -1 || prev[next] == -1)
				break;
		}
	
		routes[i] = next;
	}
}

}

