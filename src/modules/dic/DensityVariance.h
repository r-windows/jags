#ifndef DENSITY_VARIANCE_H_
#define DENSITY_VARIANCE_H_

#include <model/VarMonitor.h>
#include <graph/Node.h>

#include <vector>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores running variance of density/log density/deviance
   	 */
   	class DensityVariance : public VarMonitor {
	    DensityType const _density_type;
	public:
   	    DensityVariance(std::vector<Node const *> const &nodes, DensityType const density_type);
   	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int ch) override;
   	};
	
    }
}

#endif /* DENSITY_VARIANCE_H_ */
