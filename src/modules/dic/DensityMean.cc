#include <config.h>

#include "DensityMean.h"
#include "DensityStat.h"

using std::vector;

namespace jags {
  
    namespace dic {

	DensityMean::DensityMean(vector<Node const *> const &nodes, 
				 DensityType density_type)
	    : MeanMonitor(nodes, new DensityStat(nodes, density_type))
	{
	}
    }
}
