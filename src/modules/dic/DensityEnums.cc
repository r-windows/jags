#include "DensityEnums.h"

using std::string;

namespace jags {
    namespace dic {

	DensityType getDensityType(string const &stat)
	{
      
	    if (stat == "density") {
		return DENSITY;
	    }
	    else if (stat == "logdensity") {
		return LOGDENSITY;
	    }
	    else if (stat == "deviance") {
		return DEVIANCE;
	    }
	    else if (stat == "density_total") {
		return DENSITY_TOTAL;
	    }
	    else if (stat == "logdensity_total") {
		return LOGDENSITY_TOTAL;
	    }
	    else if (stat == "deviance_total") {
		return DEVIANCE_TOTAL;
	    }
	    return DTUNSET;
	}

	PenaltyType getPenaltyType(string const &stat)
	{
	    if (stat == "pD") {
		return PD;
	    }
	    else if (stat == "pD_total") {
		return PD_TOTAL;
	    }
	    else if (stat == "popt") {
		return POPT;
	    }
	    else {
		return PTUNSET;
	    }
	}

    }
}
