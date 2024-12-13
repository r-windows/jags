#ifndef MONITOR_STAT_H_
#define MONITOR_STAT_H_

/**
 * @short Calculate statistics for monitored nodes 
 */

#include <vector>

namespace jags {

    enum WeightType {UNWEIGHTED, SCALAR_WEIGHT, VECTOR_WEIGHT};
    
    class MonitorStat {
	const std::vector<unsigned long> _dim;
	const unsigned long _length;
	const std::vector<bool> _missing;
    public:
	/**
	 * Constructor
	 *
	 * @param dim Dimensions of the monitored statistic.
	 *
	 * @param missing boolean vector indicating which elements of the
	 * value vector are missing. Missing values are missing in all
	 * chains and all iterations.
	 */
	MonitorStat(std::vector<unsigned long> const &dim,
		    std::vector<bool> const &missing);
	/**
	 * Constructor for a vector-valued MonitorStat with no missing values
	 *
	 * @param length Length of the monitored statistic.
	 */
	MonitorStat(unsigned long);
	virtual ~MonitorStat();
	/**
	 * Returns the dimension of a single monitored value, for a
	 * single chain and a single iteration.
	 */
	std::vector<unsigned long> const &dim() const;
	/**
	 * Returns the length of a single monitored value. This is the
	 * product of the dimensions returned by MonitorStat#dim.
	 */
	unsigned long length() const;
	/**
	 * Returns a boolean vector indicating which elements of the
	 * value vector are missing (represented by the special value
	 * JAGS_NA).  A missing value is missing for all chains and
	 * all iterations.
	 */
	std::vector<bool> const &missing() const;
	/**
	 * Returns the the current value of the monitored statistic
	 * for the given chain.
	 */
	virtual std::vector<double> value(unsigned int chain) const = 0;
	/**
	 * Indicates whether the statistic is weighted and, if so,
	 * whether the weights are scalar- or vector-valued. The
	 * default implementation returns UNWEIGHTED. This information
	 * may be used by Monitor sub-classes to determine whether a
	 * weighted or unweighted summary of the statistics is
	 * required.
	 *
	 * @see MeanMonitor, VarMonitor.
	 */
	virtual WeightType weighted() const;
	/**
	 * Returns the current weight associated with the monitored
	 * statistic for the given chain.
	 *
	 * The default implementation - for unweighted statistics -
	 * returns an empty vector. A child class that uses weights
	 * must override this function and return a vector of length 1
	 * for scalar weights or of length equal to the length of the value
	 * for vector weights.
	 */
	virtual std::vector<double> weight(unsigned int chain) const;
    };

}

#endif /* MONITOR_STAT_H_ */
