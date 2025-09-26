#ifndef WEIGHTED_TRACE_MONITOR_H_
#define WEIGHTED_TRACE_MONITOR_H_

#include <model/Monitor.h>

#include <vector>

namespace jags {

    class MonitorStat;
    
    /**
     * @short Stores sampled values for a given stat
     */
    class WeightedTraceMonitor : public Monitor {
	std::vector<std::vector<double>> _values; 
	std::vector<std::vector<double>> _weight_sums;
    public:
	WeightedTraceMonitor(std::vector<Node const *> const &nodes, MonitorStat *stat);
	void update(unsigned int chain) override;
	std::vector<double> value(unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
    };
    
}

#endif /* WEIGHTED_TRACE_MONITOR_H_ */
