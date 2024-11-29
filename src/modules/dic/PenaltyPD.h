#ifndef PENALTY_PD_H_
#define PENALTY_PD_H_

#include <model/MeanMonitor.h>

#include <vector>

namespace jags {

    class RNG;
    class Node;
    
    namespace dic {
	
	class PenaltyPD : public MeanMonitor {
	public:
	    PenaltyPD(std::vector<Node const *> const &nodes,
		      std::vector<RNG *> const &rngs,
		      unsigned int nrep);
	};
	
    }
}

#endif /* PENALTY_PD_H_ */
