
#include "main.h"
#include "network.h"

int main(int argc, char **argv)
{
	unsigned long seed = (unsigned long)(time(NULL));

	ncr::Network network;
	network.generate(seed, 10, 10.);
	network.print();

	return 0;
}

