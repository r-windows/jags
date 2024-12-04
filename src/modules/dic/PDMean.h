#ifndef PD_MEAN_H_
#define PD_MEAN_H_

#include <model/MeanMonitor.h>

#include <vector>

namespace jags {

    class RNG;
    class Node;
    
    namespace dic {
	
	class PDMean : public MeanMonitor {
	public:
	    PDMean(std::vector<Node const *> const &nodes,
		   std::vector<RNG *> const &rngs,
		   unsigned int nrep);
	};
	
    }
}

#endif /* PD_MEAN_H_ */
