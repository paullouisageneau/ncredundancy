
#include "main.h"
#include "network.h"
#include "rlc.h"

int main(int argc, char **argv)
{	
	unsigned long seed = (unsigned long)(time(NULL));

/*
	ncr::Network network;
	network.generateRandom(seed, 10, 10.);
	network.print();
*/

	ncr::Rlc::Init(seed);				// Global RLC initialization
	
	const unsigned int generation = 100;		// Generation size
	
	ncr::Rlc source, sink;
	source.fill(generation);			// Prepare source
	for(unsigned int i=0; i<generation; ++i)	// Transmit combinations from source to sink
	{
		ncr::Rlc::Combination c;
		source.generate(c);
		sink.solve(c);
	}
	
	std::cout << "Generation size: " << generation << ", decoded: " << sink.decodedCount() << std::endl;
	
	ncr::Rlc::Cleanup();				// Global RLC cleanup
	return 0;
}

