#include <config.h>

#include "DensityTotalVar.h"
#include "DensityTotalStat.h"

using std::vector;

namespace jags {
    namespace dic {

	DensityTotalVar::DensityTotalVar(vector<Node const *> const &nodes, 
					 DensityType const density_type)
	    : VarMonitor(nodes, new DensityTotalStat(nodes, density_type))
	{
	}

    }
}
