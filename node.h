
#ifndef NCR_NODE_H
#define NCR_NODE_H

#include "main.h"
#include "packet.h"
#include "rlc.h"

#include <vector>
#include <queue>
#include <set>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

using namespace boost::numeric::ublas;

namespace ncr
{

class Node
{
public:
	static int PacketSize;
	static int GenerationSize;
	static double Tau;
	
	Node(int _id, double _x = 0, double _y = 0);
	~Node(void);

	double distance(const Node &node) const;
	double distance2(const Node &node) const;

	void generate(int destination, unsigned count = 1);
	void recv(const Packet &packet, int from = -1);		// Callback called when a packet is heard
	void flush(void);					// Callback called to flush a flow
	bool send(Packet &packet);				// Polling function for Network

	unsigned received(void) const;				// Return recieved count (i.e. decoded)
	unsigned seen(void) const;				// Return seen count
	void reset(void);					// Reset RLC state
	
	int id;
	double x,y;					// Position
	double alpha;					// Coding factor
	
	std::vector<int>	neighbors;		// Neighbors ids
	matrix<double> 		links;			// Link quality (0 if not neighbors)
	matrix<bool>		adjacency;		// Adjacency matrix
	std::vector<double>	alphas;

	std::vector<int>	routes;
	std::vector<int>	distances;
	
	Rlc			rlc;			// RLC coder/decoder
	double			rlcAccumulator;		// Redundancy accumulator
	int			rlcSource, rlcDestination;
	bool			forward;		// Forward-only mode ?
	double			jamming;
	
	std::set<uint64_t>	forwarded;
	std::queue<Packet>	outgoing;		// Outgoing queue
	unsigned		emitted;
	
private:
	bool pathExists(int i, int j, int distance) const;
	void getNextHops(int i, int j, std::vector<int> &nexthops) const;
	void rlcRelay(int from, int source, int destination, unsigned count = 1);
};

std::ostream &operator<< (std::ostream &s, const Node &node);

}

#endif

