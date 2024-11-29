#ifndef PENALTY_PV_H_
#define PENALTY_PV_H_

#include <model/VarMonitor.h>

#include <vector>

namespace jags {
    namespace dic {
	
   	class PenaltyPV : public VarMonitor {
	public:
   	    PenaltyPV(std::vector<Node const *> const &nodes);
   	};

    }
}

#endif /* PENALTY_PV_H_ */
