#ifndef DENSITY_POOL_MEAN_H_
#define DENSITY_POOL_MEAN_H_

#include <model/Monitor.h>
#include <graph/Node.h>

#include <vector>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores running mean values (pooled between chains) of density/log density/deviance for a given Node
	 *
	 * Note that this class is used by both NodeDensityMonitorFactory and ObsStochDensMonitorFactory
   	 */
   	class DensityPoolMean : public Monitor {
 	  protected:
   	    std::vector<Node const *> const _nodes;
   	    std::vector<double> _values; // density/log density/deviance corresponding to sampled values
	    DensityType const _density_type;  // enum is defined in model/Monitor.h
	    unsigned int const _nchain;
	    unsigned int _n;
	public:
   	    DensityPoolMean(std::vector<Node const *> const &nodes, DensityType density_type);
   	    void update(unsigned int chain) override;
	    void value(std::vector<double> &v, unsigned int chain) const override;
   	    std::vector<unsigned long> dim() const override;
   	    bool poolChains() const override;
   	    bool poolIterations() const override;
   	};
	
    }
}

#endif /* DENSITY_POOL_MEAN_H_ */
