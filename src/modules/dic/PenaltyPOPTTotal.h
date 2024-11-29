#ifndef PENALTY_POPT_TOTAL_H
#define PENALTY_POPT_TOTAL_H

#include <model/WeightedMeanMonitor.h>
#include <graph/Node.h>
#include <rng/RNG.h>

#include <vector>

/* 	This version of the POPTTotal monitor is far more efficient
	but the first few estimates of POPT will use a poorly estimated
	average weight.  Also the trace mean won't exactly correspond to the
	sum of the POPT monitor (although it is close except over a short
	run)
*/

namespace jags {
    namespace dic {

	class PenaltyPOPTTotal : public WeightedMeanMonitor {
	    unsigned int _n;
	    std::vector<double> _weights;
	public:
	    PenaltyPOPTTotal(std::vector<Node const *> const &nodes,
			     std::vector<RNG *> const &rngs,
			     unsigned int nrep);

	    void update(unsigned int chain) override;
	    ~PenaltyPOPTTotal() override;
	};

    }
}

#endif /* PENALTY_POPT_TOTAL_H */
