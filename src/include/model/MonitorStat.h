#ifndef MONITOR_STAT_H_
#define MONITOR_STAT_H_

/**
 * @short Calculate statistic for monitor nodes 
 */

#include <vector>

namespace jags {
    
    class MonitorStat {
    public:
	virtual ~MonitorStat() = default;
	/**
	 * Returns the dimension of a single monitored value, which may
	 * be replicated over chains and over iterations
	 */
	virtual std::vector<unsigned long> dim() const = 0;
	virtual std::vector<double> value(unsigned int chain) const = 0;
	virtual unsigned long length() const = 0;
	virtual std::vector<double> weight(unsigned int chain) const;
    };

}

#endif /* MONITOR_STAT_H_ */
