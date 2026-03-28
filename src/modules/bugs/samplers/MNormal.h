#ifndef MNORM_METROPOLIS_H_
#define MNORM_METROPOLIS_H_

#include <sampler/Metropolis.h>

namespace jags {

    class SingletonGraphView;

    namespace bugs {

	class MNormMetropolis : public Metropolis
	{
	    SingletonGraphView const *_gv;
	    unsigned int _chain;
	    std::vector<double> _mu;
	    std::vector<double> _Sigma;
	    double _b;
	    unsigned int _n0;
	    double _ptarget;
	    unsigned long _t;
	    double _theta, _lstep;
	    double _pmean;
	    double _delta;
	public:
	    /**
	     *
	     * @param gv GraphView object
	     *
	     * @param chain Chain number starting from zero
	     *
	     * @param prior_scale The prior expectation of the
	     *                    variance covariance matrix is a
	     *                    diagonal matrix with all diagonal
	     *                    elements set to prior_scale.
	     *
	     * @param n0 The strength of the prior for the
	     *           variance-covariance matrix is determined by
	     *           the number of pseudo-observations it
	     *           represents. This is set to n0 + d where d is
	     *           the dimension of the target distribution.
	     *
	     * @param ess_fraction. Fraction of the effective sample
	     * size that is preserved in the estimate of the posterior
	     * mean and variance.
	     *
	     * @param target_p Target probability of accepting the proposal.
	     * 
	     */
	    MNormMetropolis(SingletonGraphView const *gv, unsigned int chain,
			    double prior_scale = 1.0, unsigned int n0 = 30,
			    double ess_fraction = 0.5, double target_p = 0.234);
	    void rescale(double p) override;
	    void update(RNG *rng) override;
	    bool checkAdaptation() const override;
	    void getValue(std::vector<double> &value) const override;
	    void setValue(std::vector<double> const &value) override;
	};

    }
}

#endif /* MNORM_METROPOLIS_H_ */
