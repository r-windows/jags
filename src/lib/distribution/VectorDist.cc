#include <config.h>
#include <distribution/VectorDist.h>
#include <util/nainf.h>
//
#include <distribution/DistError.h>

using std::string;
using std::vector;

namespace jags {

VectorDist::VectorDist(string const &name, unsigned int npar)
  : Distribution(name, npar)
{

}
    void VectorDist::randomSample(double *, vector<bool> const &,
				  vector<double const *> const &,
				  vector<unsigned long> const &, 
				  RNG *) const
    {
	throw DistError(this, "Cannot sample from partially observed node");
    }

    double VectorDist::KL(vector<double const *> const &par1,
			  vector<double const *> const &par2,
			  vector<unsigned long> const &lengths,
			  RNG *rng, unsigned int nrep) const
    {
	double div = 0;

	unsigned long N = length(lengths);
	vector<double> v(N);
	for (unsigned int r = 0; r < nrep; ++r) {
	    randomSample(&v[0], par1, lengths, rng);
	    div += logDensity(&v[0], PDF_FULL, par1, lengths);
	    div -= logDensity(&v[0], PDF_FULL, par2, lengths);
	}
	return div / nrep;
    }

    double VectorDist::KL(vector<double const *> const &,
			  vector<double const *> const &,
			  vector<unsigned long> const &) const
    {
	return JAGS_NAN;
    }

    void VectorDist::score(double *, double const *,
			   vector<double const *> const &,
			   vector<unsigned long> const &,
			   unsigned long) const
    {
    }

    double VectorDist::logLikelihood(double const *x, vector<bool> const &observed,
				     vector<double const *> const &parameters,
				     vector<unsigned long> const &lengths) const
    {
	return JAGS_NAN;
    }
    
} //namespace jags
