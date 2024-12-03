#include <config.h>

#include "PenaltyPOPT.h"
#include "POPTStat.h"

using std::vector;

namespace jags {
    namespace dic {

	PenaltyPOPT::PenaltyPOPT(vector<Node const *> const &nodes,
				 vector<RNG *> const &rngs,
				 unsigned int nrep)
	    : WeightedMeanMonitor(nodes, new POPTStat(nodes, rngs, nrep))
	{
	}
	
    }
}
