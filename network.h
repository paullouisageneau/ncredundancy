
#ifndef NCR_NETWORK_H
#define NCR_NETWORK_H

#include "main.h"
#include "node.h"

#include <vector>

namespace ncr
{

class Network
{
public:
	Network(void);
	~Network(void);

	unsigned int count(void) const;

	void generate(unsigned long seed, unsigned int count, double radius);
	void print(void) const;

private:
	std::vector<Node> nodes;
};

}

#endif

