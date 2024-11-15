#ifndef DENSITY_VARIANCE_H_
#define DENSITY_VARIANCE_H_

#include <model/Monitor.h>
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
   	class DensityVariance : public Monitor {
	    std::vector<std::vector<double> > _means;
	    std::vector<std::vector<double> > _mms;
	    std::vector<std::vector<double> > _variances;
	    DensityType const _density_type;  // enum is defined in model/Monitor.h
	    unsigned int _n;
	public:
   	    DensityVariance(std::vector<Node const *> const &nodes, DensityType const density_type);
   	    void update() override;
   	    std::vector<double> const &value(unsigned int chain) const override;
   	    std::vector<unsigned long> dim() const override;
   	    bool poolChains() const override;
   	    bool poolIterations() const override;
   	};
	
    }
}

#endif /* DENSITY_VARIANCE_H_ */
