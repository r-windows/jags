#include <config.h>
#include "MNormal.h"

#include <DMNorm.h>

#include <graph/StochasticNode.h>
#include <sampler/SingletonGraphView.h>
#include <rng/RNG.h>
#include <matrix/lapack.h>
#include <matrix/matrix.h>
#include <JRmath.h>

#include <cmath>
#include <algorithm>

using std::vector;
using std::copy;
using std::exp;
using std::sqrt;
using std::min;
using std::max;
using std::string;

namespace jags {
    namespace bugs {
	
	// Target effective sample size 
	static double ESS(unsigned long t, double b, double n0) {
	    if (t <= 1) {
		return n0 + t;
	    } else {
		return n0 + 1 + b*(t-1);
	    }
	}
	
	// Returns learning rate for the sample mean and variance for
	// a given sample size t.
	static double solve_lambda(unsigned long t, double b, double n0) {
	    double S0 = ESS(t-1, b, n0);
	    double S1 = ESS(t, b, n0);
	    double delta = S1 - S0;
	    
	    return (1 + sqrt(S0*(1-delta)/S1))/(1 + S0);
	}

	static double cal_delta(double d, double a) {
	    /*
	      Optimal scaling for the Robbins-Munro algorithm to find
	      the target step size according to Garthwait, Fan, and
	      Sisson (2016) https://doi.org/10.1080/03610926.2014.936562
	    */
	    double A = - qnorm5(a/2, 0.0, 1.0, 1, 0);
	    return (1 - 1.0/d) * (sqrt(M_2PI) * exp(A*A/2)/(2*A)) + 1/(d*a*(1.0 - a));
	}
	
	static vector<double> initValue(SingletonGraphView const *gv, 
					unsigned int chain)
	{
	    double const *x = gv->node()->value(chain);
	    unsigned long d = gv->node()->length();
	    vector<double> ivalue(d);
	    copy(x, x + d, ivalue.begin());
	    return ivalue;
	}
	
	static vector<double> initSigma(SingletonGraphView const *gv,
					double prior_scale)
	{
	    /* Initialzies Variance matrix to be a diagonal matrix
	     * with diagonal elements equal to prior_scale */
	    unsigned long d = gv->length();
	    vector<double> Sigma(d*d, 0);
	    for (unsigned long i = 0; i < d; ++i) {
		Sigma[i + d * i] = prior_scale;
	    }
	    return Sigma;
	}
	
	MNormMetropolis::MNormMetropolis(SingletonGraphView const *gv, 
					 unsigned int chain,
					 double prior_scale, unsigned int n0, double ess_fraction,
					 double target_p)
	    : Metropolis(initValue(gv, chain)),
	      _gv(gv), _chain(chain), _mu(gv->length(), 0), _Sigma(initSigma(gv, prior_scale)), _b(ess_fraction),
	      _n0(n0 + gv->length()), _ptarget(target_p), _t(0), _theta(0), _lstep(0), _pmean(0), _delta(cal_delta(gv->length(), target_p))
	{
	    gv->checkFinite(chain); //Check validity of initial values
	}
	
	void MNormMetropolis::update(RNG *rng)
	{
	    double logdensity = -_gv->logFullConditional(_chain);
    
	    double const *xold = _gv->node()->value(_chain);
	    unsigned long d = _gv->length();

	    vector<double> eps(d);
	    DMNorm::randomsample(eps.data(), nullptr, _Sigma.data(), false, d, rng);

	    vector<double> xnew(d);
	    double ss = 2.38 * exp(_lstep) / sqrt(d);
	    for (unsigned int i = 0; i < d; ++i) {
		xnew[i] = xold[i] + ss * eps[i];
	    }
	    
	    setValue(xnew);
	    logdensity += _gv->logFullConditional(_chain);
	    accept(rng, exp(logdensity));
	}
	
	void MNormMetropolis::rescale(double p)
	{
	    _t++;
	    
	    double const *x = _gv->node()->value(_chain);
	    unsigned long d = _gv->length();
	    
	    // Get learning rate for updating shape
	    double lambda = solve_lambda(_t, _b, _n0);
	    
	    vector<double> dx0(d), dx1(d);
	    for (unsigned long i = 0; i < d; ++i) {
		dx0[i] = x[i] - _mu[i];
		_mu[i] += lambda * dx0[i];
		dx1[i] = x[i] - _mu[i];
	    }
	    for (unsigned long i = 0; i < d; ++i) {
		_Sigma[i + d * i] += lambda * (dx0[i]*dx1[i] - _Sigma[i + d*i]);
		for (unsigned long j = 0; j < i; ++j) {
		    double V = (dx0[i]*dx1[j] + dx0[j]*dx1[i])/2;
		    _Sigma[i + d*j] += lambda * (V - _Sigma[i + d*j]);
		    _Sigma[j + d*i] = _Sigma[i + d*j];
		}
	    }
	    
	    //Rescale step size
	    double nstep = 5.0/(_ptarget*(1 - _ptarget)) + _t;
	    _theta += _delta * (p - _ptarget)/sqrt(nstep);
	    _theta = max(0.0, _theta);
	    _lstep += (_theta - _lstep)/nstep;

	    //Monitor average step size with forgetting weights
	    _pmean += lambda * (p - _pmean);
	}
	
	bool MNormMetropolis::checkAdaptation() const
	{
	    if (_t < 2000) {
		return false;
	    }
	    if (_theta >= 0.05) {
		return abs(_pmean - _ptarget) <= 0.05;
	    }
	    return true;
	}

	void MNormMetropolis::getValue(vector<double> &value) const
	{
	    double const *v = _gv->node()->value(_chain);
	    copy(v, v + _gv->length(), value.begin());
	}
	
	void MNormMetropolis::setValue(vector<double> const &value)
	{
	    _gv->setValue(value, _chain);
	}
	
    }
}
