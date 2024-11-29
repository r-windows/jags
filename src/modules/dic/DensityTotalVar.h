#ifndef DENSITY_TOTAL_VAR_H_
#define DENSITY_TOTAL_VAR_H_

#include <model/VarMonitor.h>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores variance of total density/log density/deviance
	 *
	 * Total is defined as the sum for logdensity and deviance,
	 * and the product for the density.
   	 */
   	class DensityTotalVar : public VarMonitor {
	public:
   	    DensityTotalVar(std::vector<Node const *> const &nodes, 
			    DensityType const density_type);
   	};
    }
}

#endif /* DENSITY_TOTAL_VAR_H_ */
