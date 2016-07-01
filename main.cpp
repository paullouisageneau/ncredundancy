
#include "main.h"
#include "network.h"
#include "node.h"
#include "rlc.h"

#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_smallint.hpp>

void run(ncr::Network &network, 
	 unsigned long seed,
	 int forwarding,
	 int iterations,
	 double &lost,
	 double &redundancy
	);

int main(int argc, char **argv)
{	
	unsigned long seed = (unsigned long)(time(NULL));

	ncr::Rlc::Init(seed);				// Global RLC initialization

/*
	// ========== RLC test ==========
	const unsigned int generation = 64;		// Generation size
	
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

/*
	boost::random::mt19937 gen(seed);
	boost::uniform_01<double> uniform;
	for(int k=0; k<=50; ++k)
	{
		const int iterations = 10000;
		const unsigned int generation = 16;		// Generation size
		const double loss = 0.1;
		const double redundancy = 1. + double(k)/100.;
		
		double accumulator = 0.;
		unsigned total = 0;
		for(int i=0; i<iterations; ++i)
		{
			ncr::Rlc source, sink;
			source.fill(generation);			// Prepare source
			accumulator+= generation*redundancy;
			while(accumulator >= 1.)	// Transmit combinations from source to sink
			{
				accumulator-= 1.;
				
				ncr::Rlc::Combination c;
				source.generate(c);
				if(uniform(gen) >= loss) 
					sink.solve(c);
			}
			
			total+= sink.decodedCount();
		}
		
		double lost = 1. - double(total)/(generation*iterations);
		std::cout << redundancy << '\t' << lost << std::endl;
	}
*/

	const int x = 3;
	const int y = 3;
	
	for(int k=0; k<=20; ++k)
	{
		// Generate grid
		ncr::Network network(seed);
		network.generateGridoid(x, y, 1., 1.);
		network.setThreshold(1.5);
		
		double p = 0.1; //0.05*k;
		double alpha = 1.-0.05*k;
		
		for(int i=1; i<x; ++i)
		{
			for(int j=0; j<y; ++j)
			{
				int n = 1+i*y+j;
				network.setJamming(n, p);
			}
			
			network.setAlpha(1+i*y, alpha);
		}
		
		ncr::Node::GenerationSize = 32;
		ncr::Node::Tau = 0.01;
		
		double lost;
		double redundancy;
		run(network, seed, 0, 1000, lost, redundancy);
		
		std::cout << p << '\t' << alpha << '\t' << lost  << '\t' << redundancy;
		
		for(int j=0; j<y; ++j)
			std::cout << '\t' << network.emitted(1+y+j);
		
		std::cout << std::endl;
	}
	
	ncr::Rlc::Cleanup();	// Global RLC cleanup
	return 0;
}

void run(ncr::Network &network,
	 unsigned long seed,
	 int forwarding,
	 int iterations,
	 double &lost,
	 double &redundancy
	)
{
	boost::random::mt19937 gen(seed);
	
	const int source = 0;
	const int destination = network.count() - 1;
	
	network.update();	// compute matrices
	
	unsigned total = 0;
	for(int i=0; i<iterations; ++i)
	{
		std::set<int> temp;
		for(int n=0; n<network.count(); ++n)
		{
			network.setForwarding(n, false);
			temp.insert(n);
		}
		
		for(int n=0; n<forwarding && !temp.empty(); ++n)
		{
			std::set<int>::iterator it = temp.begin();
			boost::uniform_smallint<int> uniform(0, temp.size()-1);
			int u = uniform(gen);
			while(u--) ++it;
			network.setForwarding(*it, true);
			temp.erase(it);
		}
		
		network.setForwarding(destination, false);
		network.reset();
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
	}
	
	lost = 1. - double(total)/(ncr::Node::GenerationSize*iterations);
	
	redundancy = double(network.totalSent)/((network.count()-1)*ncr::Node::GenerationSize*iterations);
}
