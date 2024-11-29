#include <config.h>

#include "PenaltyPV.h"
#include "DensityStat.h"

using std::vector;

namespace jags {
    namespace dic {

	PenaltyPV::PenaltyPV(vector<Node const *> const &nodes)
	    : VarMonitor(nodes, new DensityStat(nodes, LOGDENSITY_TOTAL))
	{
	}

    }
}
