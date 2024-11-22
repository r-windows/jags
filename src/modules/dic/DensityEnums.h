#ifndef DENSITY_ENUMS_H_
#define DENSITY_ENUMS_H_

#include <string>

namespace jags {
    namespace dic {

	/**
	 * @short Stat types for density monitors
	 *
	 * This enum is used by some monitors/factories in the DIC module
	 * to generalise calculation of values related to the deviance
	 */
	enum DensityType {DTUNSET, DENSITY, LOGDENSITY, DEVIANCE, DENSITY_TOTAL, LOGDENSITY_TOTAL, DEVIANCE_TOTAL};

	/**
	 * @short Stat types for penalty monitors
	 *
	 * This enum is used by some monitors/factories in the DIC module
	 * to generalise calculation of values related to the 
	 */
	enum PenaltyType {PTUNSET, PD, PD_TOTAL, POPT, POPT_TOTAL, POPT_TOTAL_REP, PV};
	
	/**
	 * @short Summary types for deviance monitors
	 *
	 * This enum is used by some monitors/factories in the DIC module
	 * to generalise calculation of values related to the deviance
	 */
	//enum SummaryType {STUNSET, TRACE, MEAN, VAR, POOLMEAN, POOLVAR};
	
	/**
	* @short Process a string to return density and monitor types
	*
	* Used by NodeDensityMonitorFactory and ObsStochDensMonitorFactory
	*/
	DensityType getDensityType(std::string const &stat);
	PenaltyType getPenaltyType(std::string const &stat);
	bool isPooled (std::string const &stat);
	
	/*
		MonitorType &monitor_type, DensityType &density_type);
	bool getMonitorDensityTypes(std::string const &type, 
		MonitorType &monitor_type, DensityType &density_type);

		bool getMonitorDensityTypes(std::string const &type, 
					    MonitorType &monitor_type, DensityType &density_type);
	*/
	
}
}

#endif /* DENSITY_ENUMS_H_ */
