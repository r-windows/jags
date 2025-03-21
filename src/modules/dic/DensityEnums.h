#ifndef DENSITY_ENUMS_H_
#define DENSITY_ENUMS_H_

#include <string>

namespace jags {
    namespace dic {

	/**
	 * @short Stat types for density monitors
	 *
	 * This enum is used by some monitors/factories in the
	 * diagnostics module to generalise calculation of values
	 * related to the deviance.
	 */
	enum DensityType {DTUNSET, DENSITY, LOGDENSITY, DEVIANCE};

	/**
	 * @short Summary types for monitors in the dic module
	 */
	enum SummaryType {STUNSET, TRACE, MEAN, VAR, COV};
      
	/**
	* @short Process a string to return density type
	*/
	DensityType getDensityType(std::string const &stat);

	/**
	 * @short Process a string to return summary type
	 */
	SummaryType getSummaryType(std::string const &summary);

	/**
	 * @short Process a string to see if a stat is weighted
	 *
	 * @param stat String containing the name of a stat used in the dic module
	 *
	 * @return true if the corresponding MonitorStat object is weighted
	 */
	bool isWeighted(std::string const &stat);

	/**
	 *
	 * @short Process a string to see if a stat aggregates over nodes
	 *
	 * @param stat String containing the name of a stat used in the dic module
	 *
	 * @return true if the corresponding MonitorStat object gives a single summary
	 * statistic for all monitored nodes; false if each node has its own scalar value.
	 */
	bool isTotal(std::string const &stat);
    }
}

#endif /* DENSITY_ENUMS_H_ */
