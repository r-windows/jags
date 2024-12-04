#include <config.h>

#include "PDMean.h"
#include "PDStat.h"

using std::vector;

namespace jags {
    namespace dic {

	PDMean::PDMean(vector<Node const *> const &nodes,
		       vector<RNG *> const &rngs, unsigned int nrep)
	    : MeanMonitor(nodes, new PDStat(nodes, rngs, nrep))
	{
	}
	
    }
}
