#include <config.h>

#include "REScaledWishart2.h"
#include "Outcome.h"
#include "RESampler.h"
#include "SampleWishart.h"

#include <sampler/SingletonGraphView.h>
#include <graph/StochasticNode.h>
#include <module/ModuleError.h>
#include <rng/RNG.h>
#include <rng/TruncatedNormal.h>
#include <matrix/lapack.h>
#include <matrix/blas.h>
#include <JRmath.h>

#include <cmath>

using std::vector;
using std::sqrt;

namespace jags {
    namespace glm {

	//FIXME: Various versions of this function exist elsewhere
	static int MNormSample(vector<double> &x, 
			       vector<double> &b, vector<double> &A,
			       RNG *rng)
	{
	    int one = 1;
	    int info = 0;
	    int n = x.size();

	    //Solve A %*% x = b to get the conditional mean. The solution
	    //will be in b after the call to dposv.
	    jags_dposv("L", &n, &one, A.data(), &n, b.data(), &n, &info);
	    if (info != 0) return info;
	    
	    //After dposv, the leading nfree x nfree lower triangle of A
	    //holds the Cholesky factorization. Use it to generate a
	    //multivariate normal random vector with mean 0 and precision A
	    vector<double> eps(x.size());
	    for (auto p = eps.begin(); p != eps.end(); ++p) {
		*p = rng->normal();
	    }
	    jags_dtrsv("L", "T", "N", &n, A.data(), &n, eps.data(), &one);
	    
	    // Copy back sampled values
	    for (unsigned long i = 0; i < x.size(); ++i) {
		x[i] += b[i] + eps[i];
	    }
	    return info;
	}

	REScaledWishart2::REScaledWishart2(SingletonGraphView const *tau,
					   GLMMethod const *glmmethod)
	    : REMethod2(tau, glmmethod)
	{
	    vector<Node const*> const &par = tau->node()->parents();
	    double const *S = par[0]->value(_chain); //Prior scale
	    unsigned long nrow = par[0]->length();
	    double tdf = *par[1]->value(_chain); //Prior degrees of freedom
	    double const *x = tau->node()->value(_chain);

	    //Initialize hyper-parameter _sigma
	    _sigma = vector<double>(nrow);
	    for (unsigned int j = 0; j < nrow; ++j) {
		double a_shape = (nrow + tdf)/2.0;
		double a_rate = tdf * x[j + nrow*j] + 1.0/(S[j]*S[j]);
		double a = a_shape/a_rate; 
		_sigma[j] = sqrt(2*a);
	    }
	}

	void REScaledWishart2::updateTau(RNG *rng)
	{
	    unsigned long m = _sigma.size();
	    unsigned long m2 = m * m;
	    double tdf = *_tau->node()->parents()[1]->value(_chain);

	    //Prior 
	    double wdf = m + tdf - 1; //Degrees of freedom for Wishart
	    vector<double> R(m2, 0); //Scale matrix for Wishart
	    for (unsigned long j = 0; j < m; ++j) {
		R[j*m + j] = tdf * _sigma[j] * _sigma[j];
	    }

	    //Likelihood
	    vector<StochasticNode *> const &eps = _tau->stochasticChildren();
	    for (vector<StochasticNode*>::const_iterator p = eps.begin();
		 p != eps.end(); ++p)
	    {
		double const *Y = (*p)->value(_chain);
		double const *mu = (*p)->parents()[0]->value(_chain);
		//FIXME: We could use blas call dsyr here
		for (unsigned long j = 0; j < m; j++) {
		    for (unsigned long k = 0; k < m; k++) {
			R[j*m + k] += (Y[j] - mu[j]) * (Y[k] - mu[k]);
		    }
		}
		wdf += 1;
	    }

	    vector<double> xnew(m2);
	    sampleWishart(&xnew[0], m2, &R[0], wdf, m, rng);
	    _tau->setValue(xnew, _chain);
	}

	void REScaledWishart2::updateSigma(RNG *rng)
	{
	    vector<double> sigma0 = _sigma;
	    calDesignSigma();

	    //Prior scale
	    vector<Node const*> const &par = _tau->node()->parents();
	    double const *S = par[0]->value(_chain);

	    unsigned long m  = _z->ncol;
	    unsigned long m2 = m * m;
	    
	    //Get parameters of posterior distribution for _sigma
	    //Precision is A and mean is inverse(A) %*% b
	    //We work on a scale where the current value sigma0 is the origin
	    vector<double> A(m2, 0);
	    vector<double> b(m, 0);
	    for (unsigned int j = 0; j < m; ++j) {
		double priorprec = 1.0/(S[j] * S[j]);
		A[j * m + j] = priorprec;
		b[j] = - sigma0[j] * priorprec;
	    }

	    calCoefSigma(A.data(), b.data(), sigma0.data(), m);
	    MNormSample(_sigma, b, A, rng);
	    
	    /*
	    //Sample each sigma from its full conditional
	    //Fixme: wouldn't it be better to do block sampling here?
	    //Fixme: not reversible
	    for (unsigned int j = 0; j < m; ++j) {
		double sigma_mean  = _sigma[j] + b[j]/A[j*m+j];
		double sigma_sd = sqrt(1.0/A[j*m+j]);
		_sigma[j] = rnorm(sigma_mean, sigma_sd, rng);//FIXME: Not truncating here?
		double delta = _sigma[j] - sigma0[j];
		for (unsigned int k = 0; k < m; ++k) {
		    b[k] -= delta * A[m*j + k];
		}
	    }
	    */
	    
	    //Rescale tau
	    double const *tau0 = _tau->node()->value(_chain);
	    vector<double> scale(m);
	    for (unsigned int j = 0; j < m; ++j) {
		scale[j] = sigma0[j]/_sigma[j];
	    }
	    
	    vector<double> tau_scaled(m2);
	    for (unsigned int j = 0; j < m; ++j) {
		for (unsigned int k = 0; k < m; ++k) {
		    tau_scaled[j*m+k] = tau0[j*m+k] * scale[j] * scale[k];
		}
	    }
	    _tau->setValue(tau_scaled, _chain);
	}

	bool REScaledWishart2::isAdaptive() const
	{
	    return false;
	}
	
	void REScaledWishart2::adaptOff()
	{
	}
	
	bool REScaledWishart2::checkAdaptation() const
	{
	    return true;
	}
    }
}
