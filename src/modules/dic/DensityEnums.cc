#include "DensityEnums.h"

using std::string;

namespace jags {
    namespace dic {

	DensityType getDensityType(string const &stat)
	{
      
	    if (stat == "density" || stat == "loo_density" || stat == "density_total") {
		return DENSITY;
	    }
	    else if (stat == "logdensity" || stat == "loo_logdensity" || stat == "logdensity_total") {
		return LOGDENSITY;
	    }
	    else if (stat == "deviance" || stat == "loo_deviance" || stat == "deviance_total") {
		return DEVIANCE;
	    }
	    return DTUNSET;
	}

	SummaryType getSummaryType(string const &summary)
	{
	    if (summary == "trace") {
		return TRACE;
	    }
	    else if (summary == "mean") {
		return MEAN;
	    }
	    else if (summary == "var" || summary == "variance") {
		return VAR;
	    }
	    else if (summary == "cov") {
		return COV;
	    }
	    return STUNSET;
	}

	
	bool isWeighted(string const &stat)
	{
	    return (stat == "loo_density" ||
		    stat == "loo_logdensity" ||
		    stat == "loo_deviance" ||
		    stat == "loo_leverage");
	}

	bool isTotal(string const &stat)
	{
	    return (stat == "density_total" ||
		    stat == "logdensity_total" ||
		    stat == "deviance_total" ||
		    stat == "leverage_total");
	}

    }
}
