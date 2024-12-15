#ifndef TRACE_WEIGHT_H_
#define TRACE_WEIGHT_H_

#include <model/WeightMonitor.h>

namespace jags {

    class MonitorStat;
    
    /**
     * @short Stores sampled values for a given stat
     */
    class TraceWeight : public WeightMonitor {
	std::vector<std::vector<double>> _values; // sampled weights
    public:
	/**
	 * @short Trace monitor for probability weights
	 *
	 * Trace monitor that records the weights of the corresponding stat
	 * instead of the value. To be used in combination with a TraceMonitor
	 * to get full information about the weighted posterior.
	 *
	 * The weights are normalized to have sample mean 1.
	 */
	TraceWeight(std::vector<Node const *> const &nodes, MonitorStat *stat);
	void update(unsigned int chain) override;
	std::vector<double> value(unsigned int chain) const override;
	bool poolIterations() const override;
    };
    
}

#endif /* TRACE_MONITOR_H_ */
