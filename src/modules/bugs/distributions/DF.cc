#include <config.h>
#include "DF.h"

#include <JRmath.h>
#include <util/nainf.h>

using std::vector;

namespace jags {
namespace bugs {

DF::DF()
    : RScalarDist("df", 2, DIST_POSITIVE)
{}

bool DF::checkParameterValue (vector<double const *> const &par) const
{
    return (*par[0] > 0 && *par[1] > 0);
}

double DF::d(double x, PDFType type,
	     vector<double const *> const &par, bool give_log) const
{
    if (type == PDF_PRIOR) {
	double n = *par[0]/2, m = *par[1]/2;
	if (x < 0) {
	    return give_log ? JAGS_NEGINF : 0;
	}
	else if (x == 0) {
	    //Density at zero defined by continuity
	    return xlog0(n - 1, give_log);
	}
	else {
	    double y = (n - 1) * log(x) - (n + m) * log(1 + n * x / m);
	    return give_log ? y : exp(y);
	}
    }
    else {
	return dF(x, *par[0], *par[1], give_log);
    }
}

double DF::p(double x, vector<double const *> const &par, bool lower, 
	     bool use_log) const
{
    return pF(x, *par[0], *par[1], lower, use_log);
}

double DF::q(double p, vector<double const *> const &par, bool lower, 
	     bool log_p) const
{
    return qF(p, *par[0], *par[1], lower, log_p);
}

double DF::r(vector<double const *> const &par, RNG *rng) const
{
    return rF(*par[0], *par[1], rng);
}

    bool DF::hasScore(unsigned long i) const
    {
	return i >= 0 && i <= 2;
    }
    
    double DF::score(double y, vector<double const *> const &par,
		     unsigned long i) const
    {
	const double a = *par[0]/2;
	const double b = *par[1]/2;

	double grad = 0;
	if (i == 0) {
	    return (a-1)/y - (a+b)*a/(b + a*y);
	}
	else {
	    double grad = 0;
	    if (i == 1) {
		const double z = a*y/(b + a*y);
		grad = log(z) - (1 + b/a)*z + 1 + digamma(a + b) - digamma(a);
	    }
	    else if (i == 2) {
		const double w = b/(b + a*y); // == 1 - z;
		grad = log(w) - (1 + a/b)*w + 1 + digamma(a + b) - digamma(b);
	    }
	    return grad/2;
	}
    }

}}
