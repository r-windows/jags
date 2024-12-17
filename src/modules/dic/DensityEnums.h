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
	enum DensityType {DTUNSET, DENSITY, LOGDENSITY, DEVIANCE};

	/**
	 * @short Stat types for penalty monitors
	 *
	 * This enum is used by some monitors/factories in the DIC module
	 * to generalise calculation of values related to the 
	 */
	enum PenaltyType {PTUNSET, PD, PD_TOTAL, POPT};
	
	/**
	* @short Process a string to return density and monitor types
	*
	* Used by NodeDensityMonitorFactory and PenaltyMonitorFactory
	*/
	DensityType getDensityType(std::string const &stat);
	PenaltyType getPenaltyType(std::string const &stat);

	bool isWeighted(std::string const &stat);
	bool isTotal(std::string const &stat);
    }
}

#endif /* DENSITY_ENUMS_H_ */
