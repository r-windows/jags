#ifndef PD_TOTAL_STAT_H_
#define PD_TOTAL_STAT_H_

#include "DiagStat.h"

namespace jags {

    class RNG;
    class Node;
    
    namespace dic {
	
	class PDTotalStat : public MonitorStat {
	    std::vector<Node const *> const _nodes;
	    std::vector<RNG *> const _rngs;
	    unsigned int const _nrep;
	public:
	    PDTotalStat(std::vector<Node const *> const &nodes,
			std::vector<RNG *> const &rngs, unsigned int nrep);
	    std::vector<double>  value(unsigned int chain) const override;
	};
	
    }
}

#endif /* PD_TOTAL_STAT_H_ */
