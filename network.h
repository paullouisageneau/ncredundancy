
#ifndef NCR_NETWORK_H
#define NCR_NETWORK_H

#include "main.h"
#include "node.h"
#include "packet.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric::ublas;

namespace ncr
{

class Network
{
public:
	Network(void);
	~Network(void);

	void generateRandom(unsigned long seed, int count, double radius);
	void generateMesh(int nx, int ny, double stepx, double stepy);
	
	void print(void) const;
	unsigned int count(void) const;

	virtual double linkQualityFromDistance(double distance);	// Get q from distance, just a simple function in 1/d^2
	
	void update(void);						// Update the cached matrices and next hops

	// These functions operate from cached matrices
	void getLinkQuality(int i, std::vector<double> &result);		// Compute link quality with each neighbor q_v
	void getNeighbors(int i, std::vector<int> &result);			// Get indices of neighbors
	double linkQuality(int i, int j);				// Link quality between i and j
	bool areNeighbors(int i, int j);				// Are i and j neighbors ?			

	// Packet transmission simulation
	void sendPacket(const Packet &packet, int i);			// Simulate sending from i
	void setThreshold(double thresh);              // set the threshold of distance of neighbors
	double getThreshold ();                        // get the threshold of distance of neighbors

private:
	void computeLinkMatrix(matrix<double> &result);			// Compute matrix of q_ij link quality between i and j
	void computeAdjacencyMatrix(matrix<bool> &result);		// Compute adjacency matrix of the graph	
									//   (use link matrix and threshold)
	void computeNextHops(matrix<int> &result);			// Indices of next hops

	std::vector<Node> nodes;

	// Cached matrices
	matrix<double> links;
	matrix<bool>   adjancency;
	matrix<int>    nexthops;
	
	double threshold; // Threshold of distance to determine whether two nodes are neighbors

};

}

#endif

