#ifndef POPT_STAT_H_
#define POPT_STAT_H_

#include <model/MonitorStat.h>

#include <vector>

namespace jags {

    class RNG;
    class Node;
    
    namespace dic {

	class POPTStat : public MonitorStat {
	    const std::vector<Node const *> _nodes;
	    std::vector<RNG *> _rngs;
	    unsigned int _nrep;
	public:
	    POPTStat(std::vector<Node const *> const &nodes,
		     std::vector<RNG *> const &rngs,
		     unsigned int nrep);
	    std::vector<double> value(unsigned int chain) const override;
	    std::vector<double> weight(unsigned int chain) const override;
	    WeightType weighted() const override;
	};

    }
}

#endif /* POPT_STAT_H_ */
