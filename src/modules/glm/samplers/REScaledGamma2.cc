#include <config.h>

#include "REScaledGamma2.h"

#include <sampler/SingletonGraphView.h>
#include <graph/StochasticNode.h>
#include <rng/TruncatedNormal.h>
#include <JRmath.h>

#include <cmath>

using std::vector;
using std::sqrt;

namespace jags {
    namespace glm {

	static double SCALE(SingletonGraphView const *tau, unsigned int chain)
	{
	    return *tau->node()->parents()[0]->value(chain);
	}

	static double DF(SingletonGraphView const *tau, unsigned int chain)
	{
	    return *tau->node()->parents()[1]->value(chain);
	}

	REScaledGamma2::REScaledGamma2(SingletonGraphView const *tau,
				       GLMMethod const *glmmethod)
	    : REMethod2(tau, glmmethod)
	{
	    double S = SCALE(tau, _chain);
	    double df = DF(tau, _chain);

	    /*
	      Using a hierarchical prior for tau:
	      tau ~ dgamma(df * sigma^2/2, df/2)
	      sigma ~ dnorm(0, 1/S^2)
	      Initialize sigma^2 at its posterior mean given tau.
	    */
	    double tau0 = tau->node()->value(_chain)[0];
	    double sigma2_shape2 = 1 + df; // 2 * shape
	    double sigma2_rate2 = df * tau0 + 1/(S*S); // 2/scale
	    _sigma = sqrt(sigma2_shape2/sigma2_rate2);
	}

	void REScaledGamma2::updateTau(RNG *rng)
	{
	    const double df = DF(_tau, _chain);

	    // Prior
	    double shape = df/2.0; 
	    double rate = df * _sigma * _sigma / 2.0; // 1/scale
    
	    // Likelihood
	    vector<StochasticNode *> const &eps = _tau->stochasticChildren();
	    for (unsigned int i = 0; i < eps.size(); ++i) {
		double Y = *eps[i]->value(_chain);
		double mu = *eps[i]->parents()[0]->value(_chain);
		shape += 0.5;
		rate += (Y - mu) * (Y - mu) / 2.0;
	    }
	    
	    double tau1 = rgamma(shape, 1.0/rate, rng);
	    _tau->setValue(&tau1, 1, _chain);  
	}

	void REScaledGamma2::updateSigma(RNG *rng)
	{
	    double tau = *_tau->node()->value(_chain);
	    tau *= _sigma * _sigma;
	    
	    calDesignSigma();

	    //Prior 
	    double S = SCALE(_tau, _chain);
	    //Precision is A and mean is b/A relative to current value
	    double A = 1.0/(S*S);
	    double b = - _sigma * A;

	    //Add likelihood terms to A, b
	    calCoefSigma(&A, &b, &_sigma, 1);

	    //Sample new value of sigma
	    _sigma += rnorm(b/A, 1/sqrt(A), rng);

	    //Rescale tau
	    tau /= _sigma * _sigma;
	    _tau->setValue(&tau, 1, _chain);
	}
	
	bool REScaledGamma2::isAdaptive() const
	{
	    return false;
	}
	
	void REScaledGamma2::adaptOff()
	{
	}
	
	bool REScaledGamma2::checkAdaptation() const
	{
	    return true;
	}
	
    }
}
