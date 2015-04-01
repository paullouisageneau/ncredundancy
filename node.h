
#ifndef NCR_NODE_H
#define NCR_NODE_H

#include "main.h"
#include "packet.h"
#include "rlc.h"

#include <vector>
#include <queue>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

using namespace boost::numeric::ublas;

namespace ncr
{

class Node
{
public:
	Node(int _id, double _x = 0, double _y = 0);
	~Node(void);

	double distance(const Node &node) const;
	double distance2(const Node &node) const;

	void recv(const Packet &packet, int from = -1);	// Callback called when a packet is heard
	bool send(Packet &packet);			// Polling function for Network

	unsigned received(void) const;			// Return recieved count
	
	int id;
	double x,y;					// Position
	
	std::vector<int>	neighbors;		// neighbors ids
	matrix<bool>		adjacency;		// Adjacency matrix
	
	std::vector<double>	links;			// Link quality (0 if not neighbors)
	std::vector<int>	routes;
	std::vector<int>	distances;
	
	Rlc			rlc;			// RLC coder/decoder
	bool			accumulator;		// Redundancy accumulator
	bool			forward;		// Forward-only mode ?
	
	std::queue<Packet>	outgoing;		// Outgoing queue
	
private:
	bool pathExists(int i, int j, int distance);
	void getNextHops(int j, std::vector<int> &nexthops);
};

std::ostream &operator<< (std::ostream &s, const Node &node);

}

#endif

