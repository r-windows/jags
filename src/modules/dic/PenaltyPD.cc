#include <config.h>

#include "PenaltyPD.h"
#include "PDStat.h"

using std::vector;

namespace jags {
    namespace dic {

	PenaltyPD::PenaltyPD(vector<Node const *> const &nodes,
			     vector<RNG *> const &rngs, unsigned int nrep)
	    : MeanMonitor(nodes, new PDStat(nodes, rngs, nrep))
	{
	}

    }
}
