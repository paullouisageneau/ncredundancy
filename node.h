
#ifndef NCR_NODE_H
#define NCR_NODE_H

#include "main.h"

namespace ncr
{

class Node
{
public:
	Node(double _x = 0, double _y = 0);
	~Node(void);

	double distance(const Node &node) const;
	double distance2(const Node &node) const;

	double x,y;
};

}

#endif

