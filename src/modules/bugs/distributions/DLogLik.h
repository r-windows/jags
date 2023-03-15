#ifndef DLOGLIK_H_
#define DLOGLIK_H_

#include <distribution/ScalarDist.h>

namespace jags {
    namespace bugs {

	/**
	 * @short Pseudo-likelihood
	 * <pre>
	 * y ~ dloglik(ll)
	 * </pre>
	 *
	 * The dloglik distribution generates a log-likelihood with
	 * value ll for the stochastic parents of y. The
	 * log-likelihood is independent of the value of y.
	 * 
	 */
	class DLogLik : public ScalarDist {
	public:
	    DLogLik();
	    bool checkParameterValue(std::vector<double const *> const &parameters) 
		const override;
	    double logDensity(double const x, PDFType type,
			      std::vector<double const *> const &parameters,
			      double const *lower, double const *upper) const override;
	    double randomSample(std::vector<double const *> const &parameters,
				double const *lower, double const *upper, 
				RNG *rng) const override;
	    bool fullRank() const override;
	};
    }
}

#endif /* DLOGLIK_H_ */
