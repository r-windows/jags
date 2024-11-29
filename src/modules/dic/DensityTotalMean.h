#ifndef DENSITY_TOTAL_MEAN_H_
#define DENSITY_TOTAL_MEAN_H_

#include <model/MeanMonitor.h>

#include "DensityEnums.h"

namespace jags {

    class Node;
    
    namespace dic {

   	/**
   	 * @short Stores mean of total density/log density/deviance
	 *
	 * Total is defined as the sum for logdensity and deviance,
	 * and the product for the density.
   	 */
   	class DensityTotalMean : public MeanMonitor {
	public:
   	    DensityTotalMean(std::vector<Node const *> const &nodes, 
			     DensityType density_type);
   	};
    }
}

#endif /* DENSITY_TOTAL_MEAN_H_ */
