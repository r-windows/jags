#ifndef DENSITY_TOTAL_STAT_H_
#define DENSITY_TOTAL_STAT_H_

#include <model/MonitorStat.h>

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
   	class DensityTotalStat : public MonitorStat {
	    std::vector<Node const *> _nodes;
	    DensityType const _density_type;
	public:
   	    DensityTotalStat(std::vector<Node const *> const &nodes, 
			     DensityType const density_type);
	    std::vector<double> value(unsigned int ch) const override;
   	};
    }
}

#endif /* DENSITY_TOTAL_STAT_H_ */
