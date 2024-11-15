#ifndef PENALTY_POPT_H_
#define PENALTY_POPT_H_

#include "PenaltyPD.h"

#include <vector>

namespace jags {
namespace dic {

   class PenaltyPOPT : public PenaltyPD {
		std::vector<double> _weights;
    public:
	PenaltyPOPT(std::vector<Node const *> const &nodes,
		  std::vector<RNG *> const &rngs,
		  unsigned int nrep);

	void update() override;
    };

}}

#endif /* PENALTY_POPT_H_ */
