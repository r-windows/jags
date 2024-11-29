#include <config.h>

#include "DensityVariance.h"
#include "DensityStat.h"

using std::vector;

namespace jags {
    namespace dic {

	DensityVariance::DensityVariance(vector<Node const *> const &nodes,
					 DensityType const density_type)
	    : VarMonitor(nodes, new DensityStat(nodes, density_type))
	{
	}
    }

}
