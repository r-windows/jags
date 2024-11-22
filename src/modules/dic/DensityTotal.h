#ifndef DENSITY_TOTAL_TRACE_H_
#define DENSITY_TOTAL_TRACE_H_

#include <model/TraceMonitor.h>
#include <graph/Node.h>

#include <vector>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores trace of total density/log density/deviance
	 *
	 * Total is defined as the sum for logdensity and deviance, and the product for the density.
   	 */
   	class DensityTotalTrace : public TraceMonitor {
	    DensityType const _density_type;
	public:
   	    DensityTotalTrace(std::vector<Node const *> const &nodes, 
			      DensityType const density_type);
   	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int ch) override;
   	};
    }
}

#endif /* DENSITY_TOTAL_TRACE_H_ */
