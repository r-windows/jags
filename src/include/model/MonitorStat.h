#ifndef MONITOR_STAT_H_
#define MONITOR_STAT_H_

/**
 * @short Calculate statistics for monitored nodes 
 */

#include <vector>

namespace jags {
    
    class MonitorStat {
    public:
	virtual ~MonitorStat() = default;
	/**
	 * Returns the dimension of a single monitored value, for a
	 * single chain and a single iteration.
	 */
	virtual std::vector<unsigned long> dim() const = 0;
	/**
	 * Returns the length of a single monitored value.
	 */
	virtual unsigned long length() const = 0;
	/**
	 * Returns the the current value of the monitored statistic
	 * for the given chain.
	 */
	virtual std::vector<double> value(unsigned int chain) const = 0;
	/**
	 * Indicates whether the statistic is weighted. The default
	 * implementation returns false. This information may be used
	 * by Monitor sub-classes to determine whether a weighted or
	 * unweighted summary of the statistics is required.
	 *
	 * @see MeanMonitor, VarMonitor.
	 */
	virtual bool weighted() const;
	/**
	 * Returns the current weight associated with the monitored
	 * statistic for the given chain.
	 *
	 * The default implementation - for unweighted statistics -
	 * returns an empty vector. A child class that uses weights
	 * must override this function and return a vector of the same
	 * length as the value.
	 */
	virtual std::vector<double> weight(unsigned int chain) const;
    };

}

#endif /* MONITOR_STAT_H_ */
