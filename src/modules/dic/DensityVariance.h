#ifndef DENSITY_VARIANCE_H_
#define DENSITY_VARIANCE_H_

#include <model/VarMonitor.h>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores running variance of density/log density/deviance
   	 */
   	class DensityVariance : public VarMonitor {
	public:
   	    DensityVariance(std::vector<Node const *> const &nodes, DensityType const density_type);
   	};
	
    }
}

#endif /* DENSITY_VARIANCE_H_ */
