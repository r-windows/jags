#ifndef DENSITY_TOTAL_H_
#define DENSITY_TOTAL_H_

#include <model/TraceMonitor.h>
#include <graph/Node.h>

#include <vector>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores trace of total (sum or product) of density/log density/deviance for a given set of Nodes
	 *
	 * Note that this class is used by both NodeDensityMonitorFactory and ObsStochDensMonitorFactory
   	 */
   	class DensityTotal : public TraceMonitor {
   	    std::vector<std::vector<double>> _values; // total density/log density/deviance corresponding to sampled values
	    DensityType const _density_type;           // enum is defined in model/Monitor.h
	public:
   	    DensityTotal(std::vector<Node const *> const &nodes, 
			 DensityType const density_type);
   	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int ch);
   	};
    }
}

#endif /* DENSITY_TOTAL_H_ */
