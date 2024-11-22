#ifndef DENSITY_TRACE_H_
#define DENSITY_TRACE_H_

#include <model/TraceMonitor.h>
#include <graph/Node.h>

#include <vector>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores values of density/log density/deviance 
   	 */
   	class DensityTrace : public TraceMonitor {
	    DensityType const _density_type;
	public:
   	    DensityTrace(std::vector<Node const *> const &nodes, DensityType const density_type);
   	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int ch) override;
   	};
	
}
}

#endif /* DENSITY_TRACE_H_ */
