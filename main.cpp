
#include "main.h"
#include "network.h"
#include "node.h"
#include "rlc.h"

#include <boost/random/uniform_01.hpp>

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
	network.generateGrid(10, 1, 1., 1.);
	network.setThreshold(1.5);
	
	ncr::Node::GenerationSize = 16;
	ncr::Node::Tau = 0.01;
	
	const int source = 0;
	const int destination = network.count() - 1;
	const int iterations = 1000;
	const double forwardingProbability = 0.0;
	
	boost::random::mt19937 gen(seed);
	boost::uniform_01<double> uniform;

	// Set some nodes to forward only mode
	for(int n=0; n<network.count(); ++n)
	{
		if(n == destination)
		{
			network.setForwarding(n, false);
		}
		else {
			//network.setForwarding(n, (uniform(gen) < forwardingProbability));
		}
	}
	
	unsigned total = 0;
	for(int i=0; i<iterations; ++i)
	{
		network.update();
	 
		network.send(source, destination, ncr::Node::GenerationSize);
	
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
		
		//std::cout << "Process: " << 100*double(i+1)/double(iterations) << "%" << std::endl; 
	}
	
	std::cout << "Redundancy:  " << double(network.totalSent)/((network.count()-1)*ncr::Node::GenerationSize*iterations) << std::endl;

	std::cout << "Received: " << 100.*double(total)/(ncr::Node::GenerationSize*iterations) << "%" << std::endl;
	
	ncr::Rlc::Cleanup();				// Global RLC cleanup
	return 0;
}

