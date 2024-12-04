#ifndef PD_TOTAL_TRACE_H_
#define PD_TOTAL_TRACE_H_

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

#endif /* PD_TOTAL_TRACE_H_ */
