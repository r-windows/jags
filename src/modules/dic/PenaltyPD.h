#ifndef PENALTY_PD_H_
#define PENALTY_PD_H_

#include <model/MeanMonitor.h>
#include <graph/Node.h>
#include <rng/RNG.h>

#include <vector>


namespace jags {

    class RNG;
    
    namespace dic {
	
	class PenaltyPD : public MeanMonitor {
	    std::vector<RNG *> _rngs;
	    unsigned int _nrep;
	public:
	    PenaltyPD(std::vector<Node const *> const &nodes,
		      std::vector<RNG *> const &rngs,
		      unsigned int nrep);
	    ~PenaltyPD() override;
	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int chain) override;
	};
	
    }
}

#endif /* PENALTY_PD_H_ */
