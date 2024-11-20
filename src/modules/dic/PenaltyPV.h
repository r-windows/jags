#ifndef PENALTY_PV_H_
#define PENALTY_PV_H_

#include <model/Monitor.h>
#include <graph/Node.h>

#include <vector>

namespace jags {
    namespace dic {
	
   	class PenaltyPV : public Monitor {
	    double _mean;
	    double _mm;
	    std::vector<double> _pv;
	    unsigned int const _nchain;
	    unsigned int _n;
	public:
   	    PenaltyPV(std::vector<Node const *> const &nodes);
   	    void update(unsigned int chain) override;
	    void value(std::vector<double> &v, unsigned int chain) const override;	
   	    std::vector<unsigned long> dim() const override;
   	    bool poolChains() const override;
   	    bool poolIterations() const override;
   	};

    }
}

#endif /* PENALTY_PV_H_ */
