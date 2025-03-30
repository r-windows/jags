#include <config.h>
#include <util/dim.h>
#include <util/nainf.h>
#include <util/integer.h>

#include "DMNorm.h"

#include <matrix/blas.h>
#include <matrix/lapack.h>
#include <matrix/matrix.h>
#include <rng/RNG.h>

#include <cmath>
#include <vector>
#include <cfloat>

#include <JRmath.h>

using std::vector;

namespace jags {
namespace bugs {

    /* FIXME: copy paste from ConjugateNormal sampler */
    /* Changing x from STL vector to array */
    static int MNormSample(double *x, unsigned long nrow,
			   vector<double> &b, vector<double> &A,
			   vector<bool> const &observed, RNG *rng)
    {
	unsigned long nfree = count(observed.begin(), observed.end(), false);

	if (nfree < nrow) {
	    //Pack the leading parts of matrix A and vector b with
	    //elements corresponding to the free elements of x
	    for (unsigned long i = 0, p = 0; i < nrow; ++i) {
		if (!observed[i]) {
		    for (unsigned long j = 0, q = 0; j < nrow; ++j) {
			if (!observed[j]) {
			    A[p * nrow + q++] = A[i * nrow + j];
			}
		    }
		    b[p++] = b[i];
		}
	    }
	}

	int one = 1;
	int info = 0;
	int nf = asInteger(nfree);
	int nr = asInteger(nrow);

	//Solve A %*% x = b to get the posterior mean. The solution
	//will be in b after the call to dposv.
	jags_dposv("L", &nf, &one, A.data(), &nr, b.data(), &nr, &info);
	if (info != 0) return info;
	
	//After dposv, the leading nfree x nfree lower triangle of A
	//holds the Cholesky factorization. Use it to generate a
	//multivariate normal random vector with mean 0 and precision A
	vector<double> eps(nfree);
	for (unsigned long p = 0; p < nfree; ++p) {
	    eps[p] = rng->normal();
	}
	jags_dtrsv("L", "T", "N", &nf, A.data(), &nr, eps.data(), &one);
		  
	// Copy back sampled values
	for (unsigned long i = 0, p = 0; i < nrow; ++i) {
	    if (!observed[i]) {
		x[i] += b[p] + eps[p];
		++p;
	    }
	}
	return info;
    }

    
DMNorm::DMNorm()
  : ArrayDist("dmnorm", 2) 
{}

double DMNorm::logDensity(double const *x, PDFType type,
			  vector<double const *> const &parameters,
			  vector<vector<unsigned long> > const &dims) const
{
    double const * mu = parameters[0];
    double const * T = parameters[1];
    unsigned long m = dims[0][0];
    
    double loglik = 0;
    vector<double> delta(m);
    for (unsigned long i = 0; i < m; ++i) {
	delta[i] = x[i] - mu[i];
	loglik -= (delta[i] * T[i + i * m] * delta[i])/2;
	for (unsigned long j = 0; j < i; ++j) {
	    loglik -= (delta[i] * T[i + j * m] * delta[j]);
	}
    }

    switch(type) {
    case PDF_PRIOR:
	break;
    case PDF_LIKELIHOOD:
	loglik += logdet(T, m)/2;
	break;
    case PDF_FULL:
	loglik += logdet(T, m)/2 - m * M_LN_SQRT_2PI;
	break;
    }
    
    return loglik;
}

void DMNorm::randomSample(double *x,
			  vector<double const *> const &parameters,
			  vector<vector<unsigned long> > const &dims,
			  RNG *rng) const
{
    double const * mu = parameters[0];
    double const * T = parameters[1];
    unsigned long m = dims[0][0];
    
    randomsample(x, mu, T, true, m, rng);
}

    void DMNorm::randomSample(double *x, vector<bool> const &observed,
			      vector<double const *> const &parameters,
			      vector<vector<unsigned long>> const &dims, 
			      RNG *rng) const
    {
	unsigned long nrow = dims[0][0];
	unsigned long N = nrow*nrow;
	
	double const * mu = parameters[0];
	double const * T = parameters[1];

	vector<double> A(N);
	copy(T, T + N, A.begin());

	//Initialize missing elements to 0
	for (unsigned long i = 0; i < nrow; ++i) {
	    if (!observed[i]) x[i] = 0;
	}
	
	//FIXME: use BLAS dsymv
	vector<double> b(nrow, 0);
	for (unsigned long i = 0; i < nrow; ++i) {
	    for (unsigned long j = 0; j < nrow; ++j) {
		b[i] += T[j * nrow + i] * (mu[j] - x[j]);
	    }
	}
	
	MNormSample(x, nrow, b, A, observed, rng);
    }
    
void DMNorm::randomsample(double *x, double const *mu, double const *T,
			  bool prec, unsigned long nrow, RNG *rng)
{
  //FIXME: do something with rng

  unsigned long N = nrow*nrow;
  vector<double> Tcopy(N);
  copy(T, T + N, Tcopy.begin());
  vector<double> w(nrow);

  int info = 0;
  double worktest;
  int lwork = -1;
  int nr = asInteger(nrow);
  // Workspace query
  jags_dsyev ("V", "L", &nr, &Tcopy[0], &nr, &w[0], &worktest, &lwork, &info);
  // Now get eigenvalues/vectors with optimal work space
  lwork = static_cast<int>(worktest);
  vector<double> work(static_cast<unsigned long>(lwork));
  jags_dsyev ("V", "L", &nr, &Tcopy[0], &nr, &w[0], &work[0], &lwork, &info);

  /* Generate independent random normal variates, scaled by
     the eigen values. We reuse the array w. */
  if (prec) {
      for (unsigned long i = 0; i < nrow; ++i) {
	  w[i] = rnorm(0, 1/sqrt(w[i]), rng);
      }
  }
  else {
      for (unsigned long i = 0; i < nrow; ++i) {
	  w[i] = rnorm(0, sqrt(w[i]), rng);
      }
  }

  /* Now transform them to dependant variates 
    (On exit from DSYEV, Tcopy contains the eigenvectors)
  */
  for (unsigned long i = 0; i < nrow; ++i) {
      x[i] = mu ? mu[i] : 0;
      for (unsigned long j = 0; j < nrow; ++j) {
	  x[i] += Tcopy[i + j * nrow] * w[j]; 
      }
  }
}

bool DMNorm::checkParameterDim(vector<vector<unsigned long> > const &dims) const
{
    //Allow scalar mean and precision. 
    if (isScalar(dims[0]) && isScalar(dims[1])) return true;

    //Vector mean and matrix precision
    if (!isVector(dims[0])) return false;
    if (!isSquareMatrix(dims[1])) return false;
    if (dims[0][0] != dims[1][0]) return false;
    
    return true;
}

bool DMNorm::checkParameterValue(vector<double const *> const &,
				 vector<vector<unsigned long> > const &)
    const
{
    return true; //FIXME: Define default in base clase
}
    
vector<unsigned long> DMNorm::dim(vector<vector<unsigned long> > const &dims) const
{
    return dims[0];
}

void DMNorm::support(double *lower, double *upper,
		     vector<double const *> const &,
		     vector<vector<unsigned long> > const &dims) const
{
    unsigned long length = dims[0][0];
    for (unsigned long i = 0; i < length; ++i) {
	lower[i] = JAGS_NEGINF;
	upper[i] = JAGS_POSINF;
    }
}

bool DMNorm::isSupportFixed(vector<bool> const &) const
{
    return true;
}

bool DMNorm::hasScore(unsigned long i) const
{
    return true;
}

void DMNorm::score(double *s, double const *x,
		     vector<double const *> const &parameters,
		     vector<vector<unsigned long>> const &dims, 
		     unsigned long i) const
{

    double const * mu = parameters[0];
    double const * T = parameters[1];
    unsigned long m = dims[0][0];

    vector<double> delta(m);
    for (unsigned long j = 0; j < m; ++j) {
	delta[j] = x[j] - mu[j];
    }

    if (i == 0) {
	for (unsigned long j = 0; j < m; ++j) {
	    s[j] = 0;
	    for (unsigned long k = 0; k < j; ++k) {
		s[j] += T[j + k * m] * delta[k];
	    }
	    for (unsigned long k = j; k < m; ++k) {
		s[j] += T[k + j * m] * delta[k];
	    }
	}
    }
    else if (i == 1) {
	inverse_chol(s, T, m);
	for (unsigned long j = 0; j < m; ++j) {
	    for (unsigned long k = 0; k < m; ++k) {
		s[j + m * k] -= delta[j] * delta[k];
		s[j + m * k] /= 2;
	    }
	}
    }
}

}}
