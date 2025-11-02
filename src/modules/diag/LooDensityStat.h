#ifndef LOO_DENSITY_STAT_H_
#define LOO_DENSITY_STAT_H_

#include "DensityEnums.h"

#include <model/MonitorStat.h>

namespace jags {

    class Node;
    
    namespace diag {
	
   	/**
   	 * @short Leave-one-out (LOO) weighted version of DensityStat
	 *
	 
   	 */
   	class LooDensityStat : public MonitorStat {
	    const std::vector<Node const*> _nodes;
	    const DensityType _density_type;
	public:
	    LooDensityStat(std::vector<Node const *> const &nodes,
			   DensityType density_type);
	    std::vector<double> value(unsigned int ch) const override;
	    std::vector<double> weight(unsigned int ch) const override;
	    WeightType weighted() const override;
   	};
	
    }
}

#endif /* LOO_DENSITY_STAT_H_ */
