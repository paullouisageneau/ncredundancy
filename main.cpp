
#include "main.h"
#include "network.h"
#include "node.h"
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
	ncr::Network network(seed);
	network.generateGrid(10, 3, 1., 1.);
	network.setThreshold(1.5);
	
	ncr::Node::Tau = 0.01;
	
	const unsigned int generation = 16;		// Generation size
	const int source = 1;
	const int destination = network.count() - 2;
	const int iterations = 10;
	
	unsigned total = 0;
	for(int i=0; i<iterations; ++i)
	{
		network.update();
	
		network.send(source, destination, generation);
	
		// Verbose mode
		/*
		unsigned step = 0;
		do {
			std::cout << "---------- Step " << step++ << " ----------" << std::endl;
			network.print();
		}
		while(network.step());
		*/

		// Silent mode
		while(network.step());
	
		//network.print();
		
		total+= network.received(destination);
		network.reset();
	}
	
	std::cout << "Optimized:  " << 100.*(1.-double(network.totalSent)/(network.count()*generation*iterations)) << "%" << std::endl;
	std::cout << "Received: " << 100.*double(total)/(generation*iterations) << "%" << std::endl;
	
	ncr::Rlc::Cleanup();				// Global RLC cleanup
	return 0;
}

