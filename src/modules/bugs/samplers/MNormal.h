#ifndef MNORM_METROPOLIS_H_
#define MNORM_METROPOLIS_H_

#include <sampler/MutableSampleMethod.h>

#include <vector>

namespace jags {

    class SingletonGraphView;

    namespace bugs {

	/**
	 * Adaptive random walk Metropolis-Hastings sampler for the
	 * multivariate normal distribution, based largely on the work
	 * Spencer (2021).
	 */
	class MNormMetropolis : public MutableSampleMethod
	{
	    SingletonGraphView const *_gv;
	    unsigned int _chain;
	    std::vector<double> _mu;
	    std::vector<double> _Sigma;
	    std::vector<double> _Sigma_chol;
	    double _b;
	    unsigned int _n0;
	    double _ptarget;
	    unsigned long _t;
	    double _lstep, _lstep_bar;
	    double _pmean;
	    double _delta;
	    bool _adapt;
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
	    void rescale(double p);
	    void update(RNG *rng) override;
	    bool checkAdaptation() const override;
	    void adaptOff() override;
	    bool isAdaptive() const override;
	};

    }
}

#endif /* MNORM_METROPOLIS_H_ */
