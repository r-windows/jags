#ifndef DENSITY_TRACE_H_
#define DENSITY_TRACE_H_

#include <model/TraceMonitor.h>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores values of density/log density/deviance 
   	 */
   	class DensityTrace : public TraceMonitor {
	public:
   	    DensityTrace(std::vector<Node const *> const &nodes, DensityType const density_type);
   	};
	
}
}

#endif /* DENSITY_TRACE_H_ */
