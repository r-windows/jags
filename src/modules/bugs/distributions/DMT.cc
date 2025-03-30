#include <config.h>
#include <util/dim.h>
#include <util/nainf.h>
#include <util/integer.h>
#include "DMNorm.h"
#include "DMT.h"

#include <matrix/lapack.h>
#include <matrix/matrix.h>

#include <cmath>
#include <vector>
#include <algorithm>

#include <JRmath.h>

using std::vector;
using std::copy;

namespace jags {
namespace bugs {

DMT::DMT()
  : ArrayDist("dmt", 3) 
{}

double DMT::logDensity(double const *x, PDFType type,
		       vector<double const *> const &parameters,
		       vector<vector<unsigned long> > const &dims) const
{
    double const * mu = parameters[0];
    double const * T = parameters[1];
    double k = parameters[2][0];
    unsigned long m = dims[0][0];

    /* Calculate inner product ip = t(x - mu) %*% T %*% (x - mu) */
    double ip = 0;
    double * delta = new double[m];
    for (unsigned long i = 0; i < m; ++i) {
	delta[i] = x[i] - mu[i];
	double const *Ti = T + i*m;
	ip += (delta[i] * Ti[i] * delta[i]);
	for (unsigned long j = 0; j < i; ++j) {
	    ip += 2 * delta[i] * Ti[j] * delta[j];
	}
    }
    delete [] delta;

    double d = m; // Avoid problems with integer division
    if (type == PDF_PRIOR) {
	//No need to calculate normalizing constant
	return -((k + d)/2) * log(1 + ip/k);
    }
    else {
	return -((k + d)/2) * log(1 + ip/k) + logdet(T, m)/2 +
	    lgammafn((k + d)/2) - lgammafn(k/2) - (d/2) * log(k) - 
	    (d/2) * log(M_PI);
    }
}

void DMT::randomSample(double *x,
		       vector<double const *> const &parameters,
		       vector<vector<unsigned long> > const &dims,
		       RNG *rng) const
{

    double const * mu = parameters[0];
    double const * T = parameters[1];
    double k = *parameters[2];
    unsigned long length = dims[0][0];
    
    DMNorm::randomsample(x, mu, T, true, length, rng);
    double C = sqrt(rchisq(k, rng)/k);
    for (unsigned long i = 0; i < length; ++i) {
	x[i] = mu[i] + (x[i] - mu[i]) / C;
    }
}

bool DMT::checkParameterDim(vector<vector<unsigned long> > const &dims) const
{
  if (!isVector(dims[0]))
    return false;
  if (!isSquareMatrix(dims[1]))
    return false;
  if (dims[0][0] != dims[1][0])
    return false;
  if (!isScalar(dims[2]))
      return false;
  return true;
}

vector<unsigned long> DMT::dim(vector<vector<unsigned long> > const &dims) const
{
    return dims[0];
}

bool
DMT::checkParameterValue(vector<double const *> const &parameters,
			 vector<vector<unsigned long> > const &) const
{
    return *parameters[2] >= 2;
}


void DMT::support(double *lower, double *upper, 
		     vector<double const *> const &,
		     vector<vector<unsigned long> > const &dims) const
{
    unsigned long length = dims[0][0];
    for (unsigned long i = 0; i < length; ++i) {
	lower[i] = JAGS_NEGINF;
	upper[i] = JAGS_POSINF;
    }
}

bool DMT::isSupportFixed(vector<bool> const &) const
{
    return true;
}

    void DMT::score(double *s, double const *x,
		    vector<double const *> const &parameters,
		    vector<vector<unsigned long>> const &dims, 
		    unsigned long i) const
    {
	double const * mu = parameters[0];
	double const * T = parameters[1];
	double k = parameters[2][0];
	unsigned long m = dims[0][0];
	unsigned long N = m * m;
       
	vector<double> delta(m);
	for (unsigned long j = 0; j < m; ++j) {
	    delta[j] = x[j] - mu[j];
	}

	// eta = T %*% (x - mu)
	vector<double> eta(m);
	jags_dsymv("L", m, 1.0, T, m, delta.data(), 1, 0.0, eta.data(), 1);
       
	/* Calculate inner product ip = t(x - mu) %*% T %*% (x - mu) */
	double ip = 0;
	for (unsigned long j = 0; j < m; ++j) {
	    ip += delta[j] * eta[j];
	}

	double d = m; // Avoid problems with integer division
       
	double S = 1 + ip/k;
	double Z = (k + d)/(k * S);
	int im = asInteger(m);

	if (i == 0) {
	    for (unsigned long j = 0; j < m; ++j) {
		s[j] = Z * eta[j];
	    }
	}
	else if (i == 1) {
	    int info;
	    copy(T, T + N, s);
	    jags_dpotri("L", &im, s, &im, &info);
	    for (unsigned long j = 0; j < m; ++j) {
		for (unsigned long k = 0; k <= j; ++k) {
		    s[j + k*m] -= Z * delta[j] * delta[k];
		    if (j != k) s[k + j*m] = s[j + k*m];
		}
	    }
	    for (unsigned long j = 0; j < N; ++j) {
		s[j] /= 2;
	    }
	}
	else if (i == 2) {
	    //FIXME: check this
	    *s = (digamma((k+d)/2) - digamma(k) - d/k - (k+d) * log(S) +
		  Z*ip/k)/2;
	}
    }

}}
