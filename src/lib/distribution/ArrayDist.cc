#include <config.h>
#include <distribution/ArrayDist.h>
#include <distribution/DistError.h>
#include <util/dim.h>
#include <util/nainf.h>

using std::string;
using std::vector;
using std::fill;

namespace jags {

ArrayDist::ArrayDist(string const &name, unsigned int npar)
  : Distribution(name, npar)
{
}
    void ArrayDist::score(double *, double const *,
			  vector<double const *> const &,
			  vector<vector<unsigned long>> const &,
			  unsigned long) const
    {
    }
    
    void ArrayDist::randomSample(double *, vector<bool> const &,
				 vector<double const *> const &,
				 vector<vector<unsigned long>> const &, 
				 RNG *) const
    {
	throw DistError(this, "Cannot sample from partially observed node");
    }


    double ArrayDist::KL(vector<double const *> const &par1,
			 vector<double const *> const &par2,
			 vector<vector<unsigned long> > const &dims,
			 RNG *rng, unsigned int nrep) const
    {
	double div = 0;

	vector<unsigned long> d = dim(dims);
	unsigned long N = product(d);
	vector<double> v(N);
	for (unsigned int r = 0; r < nrep; ++r) {
	    randomSample(&v[0], par1, dims, rng);
	    div += logDensity(&v[0], PDF_FULL, par1, dims);
	    div -= logDensity(&v[0], PDF_FULL, par2, dims);
	}
	return div / nrep;
    }

    double ArrayDist::KL(vector<double const *> const &,
			 vector<double const *> const &,
			 vector<vector<unsigned long> > const &) const
    {
	return JAGS_NAN;
    }

    double ArrayDist::logLikelihood(double const *x, vector<bool> const &observed,
				    vector<double const *> const &parameters,
				    vector<vector<unsigned long>> const &dims) const
    {
	throw DistError(this, "Cannot calculate likelihood for partially observed node");
    }

} //namespace jags
