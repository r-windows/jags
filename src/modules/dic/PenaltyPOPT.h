#ifndef PENALTY_POPT_H_
#define PENALTY_POPT_H_

#include <model/WeightedMeanMonitor.h>

#include <vector>

namespace jags {

    class RNG;
    
    namespace dic {

	class PenaltyPOPT : public WeightedMeanMonitor {
	    std::vector<RNG *> _rngs;
	    unsigned int _nrep;
	public:
	    PenaltyPOPT(std::vector<Node const *> const &nodes,
			std::vector<RNG *> const &rngs,
			unsigned int nrep);
	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int chain) override;
	    std::vector<double> weight(unsigned int chain) override;
	};

    }
}

#endif /* PENALTY_POPT_H_ */
