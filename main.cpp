
#include "main.h"
#include "network.h"
#include "rlc.h"

int main(int argc, char **argv)
{	
	unsigned long seed = (unsigned long)(time(NULL));

	ncr::Rlc::Init(seed);				// Global RLC initialization

/*
	// ========== RLC test ==========
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
*/
	
	// Generate grid
	ncr::Network network;
	network.generateGrid(3, 3, 1., 1.);
	network.setThreshold(1.5);
	
	network.update();
	
	const unsigned int generation = 10;	// Generation size
	network.send(0, 8, generation);
	
	// Silent mode
	// while(network.step());
	
	// Verbose mode
	unsigned step = 0;
	do {
		std::cout << "---------- Step " << step++ << " ----------" << std::endl;
		network.print();
	}
	while(network.step());
	
	double loss = 1. - double(network.received(8))/generation;
	std::cout << "Received " << network.received(8) <<", loss=" << loss << "%" << std::endl;
	
	ncr::Rlc::Cleanup();				// Global RLC cleanup
	return 0;
}

