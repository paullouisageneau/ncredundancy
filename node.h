
#ifndef NCR_NODE_H
#define NCR_NODE_H

#include "main.h"
#include "packet.h"

#include <queue>
#include <map>

namespace ncr
{

class Node
{
public:
	Node(double _x = 0, double _y = 0);
	~Node(void);

	double distance(const Node &node) const;
	double distance2(const Node &node) const;

	void recv(const Packet &packet);	// Callback called when a packet is heard
	bool send(Packet &packet);		// Polling function for Network

	double x,y;				// Position
	std::vector<Node> 	neighbors;	// Neighbor nodes
	std::vector<double>	link;		// *Measured* link quality
	
	bool			forward;	// Forward-only mode ?
	std::map<int, Packet>	incoming;	// Incoming buffer, sorted by pivot to enable solving
	std::queue<Packet>	outgoing;	// Outgoing queue
};

}

#endif

