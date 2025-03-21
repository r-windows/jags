#ifndef LEVERAGE_STAT_H_
#define LEVERAGE_STAT_H_

#include <model/MonitorStat.h>

namespace jags {

    class RNG;
    class Node;
    
    namespace diag {
	
	class LeverageStat : public MonitorStat {
	    std::vector<Node const *> const _nodes;
	    std::vector<RNG *> _rngs;
	    unsigned int _nrep;
	public:
	    LeverageStat(std::vector<Node const *> const &nodes,
			 std::vector<RNG *> const &rngs,
			 unsigned int nrep);
	    std::vector<double> value(unsigned int chain) const override;
	};
	
    }
}

#endif /* LEVERAGE_STAT_H_ */
