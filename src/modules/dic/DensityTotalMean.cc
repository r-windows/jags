#include <config.h>

#include "DensityTotalMean.h"
#include "DensityTotalStat.h"

using std::vector;

namespace jags {
    namespace dic {
	
	DensityTotalMean::DensityTotalMean(vector<Node const *> const &nodes, 
					   DensityType const density_type)
	    : MeanMonitor(nodes, new DensityTotalStat(nodes, density_type))
	{
	}

    }
}
