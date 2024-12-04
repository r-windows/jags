#include <config.h>

#include "POPTMean.h"
#include "POPTStat.h"

using std::vector;

namespace jags {
    namespace dic {

	POPTMean::POPTMean(vector<Node const *> const &nodes,
			   vector<RNG *> const &rngs,
			   unsigned int nrep)
	    : WeightedMeanMonitor(nodes, new POPTStat(nodes, rngs, nrep))
	{
	}
	
    }
}
