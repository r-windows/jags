#include <config.h>

#include "DensityTrace.h"
#include "DensityStat.h"

using std::vector;

namespace jags {
    namespace dic {

	DensityTrace::DensityTrace(vector<Node const *> const &nodes,
				   DensityType const density_type)
	    : TraceMonitor(nodes, new DensityStat(nodes, density_type))
	{
	}
	
    }

}
