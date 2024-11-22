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
	else if (stat == "popt_total") {
	    return POPT_TOTAL;
	}
	else if (stat == "pV") {
	    return PV;
	}
	else {
	    return PTUNSET;
	}
    }
    
    bool isPooled(string const &stat) {

	return stat == "density_total" ||
	    stat == "logdensity_total" ||
	    stat == "deviance_total" ||
	    stat == "pD_total" ||
	    stat == "popt_total" ||
	    stat == "popt_total_replicatemean";
    }
    
    /*
	bool getMonitorDensityTypes(string const &type, MonitorType &monitor_type, 
				    DensityType &density_type)
	{

		if (type == "density_trace") {
		    summary_type = TRACE;
		    stat_type = DENSITY;
		}
		else if (type == "density_mean") {
		    summary_type = MEAN;
		    stat_type = DENSITY;
		}
		else if (type == "density_variance") {
		    summary_type = VARIANCE;
		    stat_type = DENSITY;
		}
		else if (type == "density_total") {
		    stat_type = TOTAL;
		    density_type = DENSITY;
		}
		else if (type == "density_poolmean") {
		    monitor_type = POOLMEAN;
		    density_type = DENSITY;
		}
		else if (type == "density_poolvariance") {
		    monitor_type = POOLVARIANCE;
		    density_type = DENSITY;
		}
		else if (type == "logdensity_trace") {
		    summary_type = TRACE;
		    density_type = LOGDENSITY;
		}
		else if (type == "logdensity_mean") {
		    monitor_type = MEAN;
		    density_type = LOGDENSITY;
		}
		else if (type == "logdensity_variance") {
		    monitor_type = VARIANCE;
		    density_type = LOGDENSITY;
		}
		else if (type == "logdensity_total") {
		    monitor_type = TOTAL;
		    density_type = LOGDENSITY;
		}
		else if (type == "logdensity_poolmean") {
		    monitor_type = POOLMEAN;
		    density_type = LOGDENSITY;
		}
		else if (type == "logdensity_poolvariance") {
		    monitor_type = POOLVARIANCE;
		    density_type = LOGDENSITY;
		}
		else if (type == "deviance_trace") {
		    summary_type = TRACE;
		    density_type = DEVIANCE;
		}
		else if (type == "deviance_mean") {
		    monitor_type = MEAN;
		    density_type = DEVIANCE;
		}
		else if (type == "deviance_variance") {
		    monitor_type = VARIANCE;
		    density_type = DEVIANCE;
		}
		else if (type == "deviance_total") {
		    monitor_type = TOTAL;
		    density_type = DEVIANCE;
		}
		else if (type == "deviance_poolmean") {
		    monitor_type = POOLMEAN;
		    density_type = DEVIANCE;
		}
		else if (type == "deviance_poolvariance") {
		    monitor_type = POOLVARIANCE;
		    density_type = DEVIANCE;
		}
		else if (type == "pD") {
		    monitor_type = PD;
		}
		else if (type == "pD_total") {
		    monitor_type = PDTOTAL;
		}
		else if (type == "popt") {
			monitor_type = POPT;
		}
		else if (type == "popt_total") {
			monitor_type = POPTTOTAL;
		}
		else if (type == "popt_total_replicatemean") {
			monitor_type = POPTTOTALREP;
		}
		else if (type == "pv") {
			monitor_type = PV;
		}
		else {
		    // If not listed above:
		    return false;
		}
	
		return true;
		
	}
    */
}}
