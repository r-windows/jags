#ifndef DENSITY_STAT_H_
#define DENSITY_STAT_H_

#include <model/MonitorStat.h>

#include "DensityEnums.h"

namespace jags {

    class Node;
    
    namespace dic {
	
   	/**
   	 * @short Stores running mean values of density/log density/deviance
   	 */
   	class DensityStat : public MonitorStat {
	    std::vector<Node const*> const _nodes;
	    DensityType const _density_type;
	public:
	    DensityStat(std::vector<Node const *> const &nodes,
			DensityType density_type);
	    std::vector<unsigned long> dim() const override;
	    std::vector<double> value(unsigned int ch) const override;
	    unsigned long length() const override;
   	};
	
    }
}

#endif /* DENSITY_STAT_H_ */
