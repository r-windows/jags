#ifndef PENALTY_PV_H_
#define PENALTY_PV_H_

#include <model/VarMonitor.h>
#include <graph/Node.h>

#include <vector>

namespace jags {
    namespace dic {
	
   	class PenaltyPV : public VarMonitor {
	public:
   	    PenaltyPV(std::vector<Node const *> const &nodes);
	    std::vector<double> stat(unsigned int chain) override;
   	    std::vector<unsigned long> dim() const override;
   	};

    }
}

#endif /* PENALTY_PV_H_ */
