#ifndef POPT_MEAN_H_
#define POPT_MEAN_H_

#include <model/WeightedMeanMonitor.h>

#include <vector>

namespace jags {

    class RNG;
    class Node;
    
    namespace dic {

	class POPTMean : public WeightedMeanMonitor {
	public:
	    POPTMean(std::vector<Node const *> const &nodes,
		     std::vector<RNG *> const &rngs,
		     unsigned int nrep);
	};

    }
}

#endif /* PENALTY_POPT_H_ */
