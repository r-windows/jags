#ifndef DENSITY_MEAN_H_
#define DENSITY_MEAN_H_

#include <model/MeanMonitor.h>
#include <graph/Node.h>

#include <vector>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores running mean values of density/log density/deviance
   	 */
   	class DensityMean : public MeanMonitor {
	    DensityType const _density_type;
   	  public:
   	    DensityMean(std::vector<Node const *> const &nodes, DensityType const density_type);
   	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int ch) override;
   	};
	
    }
}

#endif /* DENSITY_MEAN_H_ */
