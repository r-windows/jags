#include <config.h>

#include "PenaltyPDTotal.h"
#include "PDTotalStat.h"

using std::vector;

namespace jags {
    namespace dic {
	
	PDTotalTrace::PDTotalTrace(vector<Node const *> const &nodes,
				   vector<RNG *> const &rngs, unsigned int nrep)
	    : TraceMonitor(nodes, new PDTotalStat(nodes, rngs, nrep))
	{
	}

    }
}
