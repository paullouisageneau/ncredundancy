
#ifndef NCR_NODE_H
#define NCR_NODE_H

#include "main.h"
#include "packet.h"
#include "rlc.h"

#include <queue>
#include <map>

namespace ncr
{

class Node
{
public:
	Node(int _id, double _x = 0, double _y = 0);
	~Node(void);

	double distance(const Node &node) const;
	double distance2(const Node &node) const;

	bool recv(const Packet &packet);		// Callback called when a packet is heard, returns true if nexthops should be computed and relay called
	void relay(const Packet &packet, 		// Callback called with nexthops if recv returns true
		const std::vector<int> &nexthops);	
	bool send(Packet &packet);			// Polling function for Network

	unsigned received(void) const;			// Return recieved count
	
	int id;
	double x,y;					// Position
	std::vector<double>	link;			// *Measured* link quality
	
	Rlc			rlc;			// RLC coder/decoder
	bool			forward;		// Forward-only mode ?
	
	std::queue<Packet>	outgoing;		// Outgoing queue
};

}

#endif

