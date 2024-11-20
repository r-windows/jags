#ifndef DENSITY_VARIANCE_H_
#define DENSITY_VARIANCE_H_

#include <model/VarMonitor.h>
#include <graph/Node.h>

#include <vector>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores running variance values of density/log density/deviance for a given Node
	 *
	 * Note that this class is used by both NodeDensityMonitorFactory and ObsStochDensMonitorFactory
   	 */
   	class DensityVariance : public VarMonitor {
	    DensityType const _density_type;  // enum is defined in model/Monitor.h
	public:
   	    DensityVariance(std::vector<Node const *> const &nodes, DensityType const density_type);
   	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int ch);
   	};
	
    }
}

#endif /* DENSITY_VARIANCE_H_ */
