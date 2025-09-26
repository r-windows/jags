#ifndef WEIGHT_MONITOR_H_
#define WEIGHT_MONITOR_H_

#include <model/Monitor.h>

namespace jags {

    class MonitorStat;
    
    /**
     * @short Parent class for Monitors of probability weights
     */
    class WeightMonitor : public Monitor {
    protected:
	const std::vector<bool> _missing;
    public:
	/**
	 * Constructor
	 *
	 * @param nodes Monitored nodes which are passed to the parent
	 * class Monitor
	 *
	 * @param stat MonitorStat object which must be weighted. A logic error
	 * is thrown if the stat is unweighted. The stat should be dynamically
	 * allocated as the WeightMonitor takes ownership of it and will delete
	 * it on exit
	 */
	WeightMonitor(std::vector<Node const *> const &nodes,
		      MonitorStat *stat);
	/**
	 * If the stat has scalar weight then the length of the weight
	 * monitor is 1, otherwise for vector weights the length is the
	 * same as the length of the stat.
	 */
	unsigned long length() const override;
	/**
	 * As with the length function, the dim of a weight monitor
	 * depends whether the weight of the stat is scalar or vector.
	 * If weight is scalar then dim is scalar, otherwise it inherits
	 * the dim of the stat.
	 */
	std::vector<unsigned long> dim() const override;
	/**
	 * Weight monitors do not pool chains
	 */
	bool poolChains() const override;
	/**
	 * If weight is scalar then returns an empty vector, otherwise returns
	 * the elementNames of the stat.
	 */
	std::vector<std::string> elementNames() const override;
    };

}

#endif /* ESS_WEIGHT_H_ */
