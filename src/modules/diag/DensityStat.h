#ifndef DENSITY_STAT_H_
#define DENSITY_STAT_H_

#include <model/MonitorStat.h>

#include "DensityEnums.h"

namespace jags {

    class Node;
    
    namespace diag {
	
   	/**
   	 * @short Stores running mean values of density/log density/deviance
   	 */
   	class DensityStat : public MonitorStat {
	    const std::vector<Node const*> _nodes;
	    const DensityType _density_type;
	public:
	    DensityStat(std::vector<Node const *> const &nodes,
			DensityType density_type);
	    std::vector<double> value(unsigned int ch) const override;
   	};
	
    }
}

#endif /* DENSITY_STAT_H_ */
