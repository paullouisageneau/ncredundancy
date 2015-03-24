
#include "node.h"

namespace ncr
{

Node::Node(double _x, double _y) :
	x(_x),
	y(_y),
	forward(false)
{

}

Node::~Node(void)
{

}

double Node::distance(const Node &node) const
{
	return std::sqrt(distance2(node));
}

double Node::distance2(const Node &node) const
{
	const double dx = x-node.x;
	const double dy = y-node.y;
	return dx*dx + dy*dy;
}

}

