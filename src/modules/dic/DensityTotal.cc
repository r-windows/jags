#include <config.h>

#include "DensityTotal.h"
#include "DensityTotalStat.h"

using std::vector;

namespace jags {
    namespace dic {

	DensityTotalTrace::DensityTotalTrace(vector<Node const *> const &nodes,
					     DensityType const density_type)
	    : TraceMonitor(nodes, new DensityTotalStat(nodes, density_type))
	{
	}
	
    }
    
}
