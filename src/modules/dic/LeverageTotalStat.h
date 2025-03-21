#ifndef LEVERAGE_TOTAL_STAT_H_
#define LEVERAGE_TOTAL_STAT_H_

#include <model/MonitorStat.h>

namespace jags {

    class RNG;
    class Node;
    
    namespace diag {
	
	class LeverageTotalStat : public MonitorStat {
	    std::vector<Node const *> const _nodes;
	    std::vector<RNG *> const _rngs;
	    unsigned int const _nrep;
	public:
	    LeverageTotalStat(std::vector<Node const *> const &nodes,
			      std::vector<RNG *> const &rngs,
			      unsigned int nrep);
	    std::vector<double>  value(unsigned int chain) const override;
	};
	
    }
}

#endif /* LEVERAGE_TOTAL_STAT_H_ */
