#include <config.h>

#include "REGamma2.h"

#include <JRmath.h>
#include <sampler/SingletonGraphView.h>
#include <graph/StochasticNode.h>
#include <module/ModuleError.h>

#include <cmath>

using std::vector;
using std::sqrt;


namespace jags {

    //Utility functions used by the constructor

    static inline double const *
    SHAPE(SingletonGraphView const *tau, unsigned int chain)
    {
	return tau->node()->parents()[0]->value(chain);
    }

    static inline double const *
    RATE(SingletonGraphView const *tau, unsigned int chain)
    {
	return tau->node()->parents()[1]->value(chain);
    }

    static inline double 
    SIGMA(SingletonGraphView const *tau, unsigned int chain)
    {
	return 1.0/sqrt(*tau->node()->value(chain));
    }

    namespace glm {

	class Outcome;
	
	REGamma2::REGamma2(SingletonGraphView const *tau,
			   GLMMethod const *glmmethod)
	    : REMethod2(tau, glmmethod),
	      _slicer(this, SHAPE(tau, _chain), RATE(tau, _chain),
		      SIGMA(tau, _chain))
	{
	}

	void REGamma2::updateTau(RNG *rng)
	{
	    double shape = *SHAPE(_tau, _chain);
	    double rate = *RATE(_tau, _chain); //(1/scale)

	    // Likelihood
	    vector<StochasticNode *> const &eps = _tau->stochasticChildren();
	    for (auto p = eps.begin(); p != eps.end(); ++p) {
		double Y = *(*p)->value(_chain);
		double mu = *(*p)->parents()[0]->value(_chain);
		shape += 0.5;
		rate += (Y - mu) * (Y - mu) / 2.0;
	    }
	    
	    double tau1 = rgamma(shape, 1.0/rate, rng);
	    _tau->setValue(&tau1, 1, _chain);  
	}

	void REGamma2::updateSigma(RNG *rng)
	{
	    double tau0 = _tau->node()->value(_chain)[0];
	    double sigma0 = 1/sqrt(tau0);

	    calDesignSigma();
	    
	    _slicer.setSigma(sigma0);
	    _slicer.update(rng);
	    double sigma1 = _slicer.value();

	    //Set new value of precision parameter
	    double tau1 = 1/(sigma1 * sigma1);
	    _tau->setValue(&tau1, 1, _chain);
	}

	bool REGamma2::isAdaptive() const
	{
	    return true;
	}
	
	void REGamma2::adaptOff()
	{
	    _slicer.adaptOff();
	}
	
	bool REGamma2::checkAdaptation() const
	{
	    return _slicer.checkAdaptation();
	}

    }
}
