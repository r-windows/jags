#ifndef PENALTY_PD_TOTAL_H_
#define PENALTY_PD_TOTAL_H_

#include <model/TraceMonitor.h>
#include <graph/Node.h>
#include <rng/RNG.h>

#include <vector>

namespace jags {
    namespace dic {
	
	class PDTotalTrace : public TraceMonitor {
	    std::vector<RNG *> _rngs;
	    unsigned int _nrep;
	public:
	    PDTotalTrace(std::vector<Node const *> const &nodes,
			 std::vector<RNG *> const &rngs, unsigned int nrep);
	    ~PDTotalTrace() override;
	    std::vector<unsigned long> dim() const override;
	    std::vector<double>  stat(unsigned int chain) override;
	};
	
    }
}

#endif /* PENALTY_PD_TOTAL_H_ */
