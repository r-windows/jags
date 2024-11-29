#ifndef PENALTY_PD_TOTAL_H_
#define PENALTY_PD_TOTAL_H_

#include <model/TraceMonitor.h>

#include <vector>

namespace jags {

    class RNG;
    class Node;
    
    namespace dic {
	
	class PDTotalTrace : public TraceMonitor {
	public:
	    PDTotalTrace(std::vector<Node const *> const &nodes,
			 std::vector<RNG *> const &rngs, unsigned int nrep);
	};
	
    }
}

#endif /* PENALTY_PD_TOTAL_H_ */
