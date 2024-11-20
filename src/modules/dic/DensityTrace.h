#ifndef DENSITY_TRACE_H_
#define DENSITY_TRACE_H_

#include <model/TraceMonitor.h>
#include <graph/Node.h>

#include <vector>

#include "DensityEnums.h"

namespace jags {
    namespace dic {
	
   	/**
   	 * @short Stores values of density/log density/deviance corresponding to sampled values of a given Node
	 *
	 * Note that this class is used by both NodeDensityMonitorFactory and ObsStochDensMonitorFactory
   	 */
   	class DensityTrace : public TraceMonitor {
 	  protected:
   	    std::vector<std::vector<double>> _values; // density/log density/deviance corresponding to sampled values
	    DensityType const _density_type;  // enum is defined in model/Monitor.h
	public:
   	    DensityTrace(std::vector<Node const *> const &nodes, DensityType const density_type);
   	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int ch);
   	};
	
}
}

#endif /* DENSITY_TRACE_H_ */
