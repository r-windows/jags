#ifndef DENSITY_MEAN_H_
#define DENSITY_MEAN_H_

#include <model/MeanMonitor.h>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores running mean values of density/log density/deviance
   	 */
   	class DensityMean : public MeanMonitor {
	public:
   	    DensityMean(std::vector<Node const *> const &nodes,
			DensityType density_type);
   	};
	
    }
}

#endif /* DENSITY_MEAN_H_ */
