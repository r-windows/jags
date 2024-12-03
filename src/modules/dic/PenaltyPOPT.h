#ifndef PENALTY_POPT_H_
#define PENALTY_POPT_H_

#include <model/WeightedMeanMonitor.h>

#include <vector>

namespace jags {

    class RNG;
    class Node;
    
    namespace dic {

	class PenaltyPOPT : public WeightedMeanMonitor {
	public:
	    PenaltyPOPT(std::vector<Node const *> const &nodes,
			std::vector<RNG *> const &rngs,
			unsigned int nrep);
	};

    }
}

#endif /* PENALTY_POPT_H_ */
