#ifndef DENSITY_POOLVARIANCE_H_
#define DENSITY_POOLVARIANCE_H_

#include <model/Monitor.h>
#include <graph/Node.h>

#include <vector>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores running variance values (pooled between chains) of density/log density/deviance for a given Node
	 *
	 * Note that this class is used by both NodeDensityMonitorFactory and ObsStochDensMonitorFactory
   	 */
   	class DensityPoolVariance : public Monitor {
   	    std::vector<Node const *> const _nodes;
		std::vector<double> _means;
		std::vector<double> _mms;
		std::vector<double> _variances;
		DensityType const _density_type;  // enum is defined in model/Monitor.h
		unsigned int const _nchain;
		unsigned int _n;
   	  public:
   	    DensityPoolVariance(std::vector<Node const *> const &nodes, 
				DensityType const density_type);
   	    void update(unsigned int chain) override;
	    void value(std::vector<double> &v, unsigned int chain) const override;
   	    std::vector<unsigned long> dim() const override;
   	    bool poolChains() const override;
   	    bool poolIterations() const override;
   	};
	
}
}

#endif /* DENSITY_POOLVARIANCE_H_ */
