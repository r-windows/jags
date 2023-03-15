#include <config.h>
#include <util/nainf.h>
#include <module/ModuleError.h>

#include "DLogLik.h"

using std::vector;

namespace jags {
    namespace bugs {

	DLogLik::DLogLik()
	    : ScalarDist("dloglik", 1, DIST_UNBOUNDED)
	{
	}

	bool 
	DLogLik::checkParameterValue(vector<double const *> const &) const
	{
	    return true;
	}

	double 
	DLogLik::logDensity(double y, PDFType,
			    vector<double const *> const &par,
			    double const *, double const *) const
	{
	    return *par[0];
	}

	double DLogLik::randomSample(vector<double const *> const &par,
				     double const *, double const *,RNG *) const
	{
	    throwDistError(this, "Cannot forward sample");
	}
    
	bool DLogLik::fullRank() const
	{
	    return false;
	}	
    }
}
