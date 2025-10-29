#ifndef SCALAR_STOCHASTIC_NODE_H_
#define SCALAR_STOCHASTIC_NODE_H_

#include <graph/StochasticNode.h>

namespace jags {

class ScalarDist;

/**
 * @short Scalar Node defined by the BUGS-language operator ~
 */
class ScalarStochasticNode : public StochasticNode {
    ScalarDist const * const _dist;
    void sp(double *lower, double *upper, unsigned int chain) const override;
public:
    /**
     * Constructs a new ScalarStochasticNode 
     */
    ScalarStochasticNode(ScalarDist const *dist, unsigned int nchain,
			 std::vector<Node const *> const &parameters,
			 Node const *lower, Node const *upper);
    double logDensity(unsigned int chain, PDFType type) const override;
    double logLikelihood(unsigned int chain) const override;
    void randomSample(RNG *rng, unsigned int chain) override;
    void truncatedSample(RNG *rng, unsigned int chain,
			 double const *lower, double const *upper);
    bool checkParentValues(unsigned int chain) const override;
    //StochasticNode *clone(std::vector<Node const *> const &parents,
    //Node const *lower, Node const *upper) const;
    double KL(unsigned int ch1, unsigned int ch2, RNG *rng,
	      unsigned int nrep) const override;
};

} /* namespace jags */

#endif /* SCALAR_STOCHASTIC_NODE_H_ */

