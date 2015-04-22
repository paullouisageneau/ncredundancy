
#ifndef NCR_NETWORK_H
#define NCR_NETWORK_H

#include "main.h"
#include "node.h"
#include "packet.h"

#include <vector>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/random/mersenne_twister.hpp>

using namespace boost::numeric::ublas;

namespace ncr
{

class Network
{
public:
	Network(unsigned long seed);
	~Network(void);
	
	// Set up threshold of distance for neighbors
	void setThreshold(double thresh);	// set the threshold of distance of neighbors
	double getThreshold(void);		// get the threshold of distance of neighbors

	void generateRandom(unsigned long seed, int count, double radius);
	void generateGrid(int nx, int ny, double stepx, double stepy);
	
	void print(void) const;
	int count(void) const;

	static double linkQualityFromDistance(double distance);	// Get q from distance
								// TODO: should be virtual and non-static
	
	void update(void);						// Update the cached matrices and routing
	void send(int source, int destination, unsigned count = 1);	// Send packets from source
	bool step(void);						// Simulation step
	void reset(void);						// Reset simulation
	unsigned received(int i) const;					// Count received
	unsigned seen(int i) const;					// Count seen
	
	void setForwarding(int i, bool enabled);			// Enable forward-only mode for i
	bool forwarding(int i) const;					// Is forward-only mode enabled for i ?
	
	// These functions operate from cached matrices
	void getLinkQuality(int i, std::vector<double> &result);	// Compute link quality with each neighbor q_v
	void getNeighbors(int i, std::vector<int> &result);		// Get indices of neighbors
	double linkQuality(int i, int j);				// Link quality between i and j
	bool areNeighbors(int i, int j);				// Are i and j neighbors ?			
	
	// Stats
	unsigned int totalSent;
	unsigned int totalReceived;
	unsigned int totalLost;
	
private:
	void sendPacket(const Packet &packet, int sender);			// Simulate sending
	
	void computeLinkMatrix(matrix<double> &result);				// Compute matrix of q_ij link quality between i and j
	void computeAdjacencyMatrix(matrix<bool> &result);			// Compute adjacency matrix of the graph	
										//   (use link matrix and threshold)
	void computeRouting(int s, std::vector<int> &routes, std::vector<int> &distances);	// Run Dijkstra's algorithm on s (distances returned in hops)
	
	boost::mt19937 gen;
	
	std::vector<Node> nodes;
	
	// Cached matrices
	matrix<double> links;
	matrix<bool>   adjacency;
	matrix<int>    routes;		// next hops for routing (one per source-destination pair)
	matrix<int>    distances;	// in hops
	
	double threshold; 		// Threshold of distance to determine whether two nodes are neighbors
	
	int nextStepNode;
};

}

#endif

