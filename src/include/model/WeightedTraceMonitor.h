#ifndef WEIGHTED_TRACE_MONITOR_H_
#define WEIGHTED_TRACE_MONITOR_H_

#include <model/Monitor.h>

#include <vector>

namespace jags {
    
    /**
     * @short Stores sampled values for a given stat
     */
    class WeightedTraceMonitor : public Monitor {
	std::vector<std::vector<double>> _values; // sampled values
	std::vector<std::vector<double>> _weight_sums; // sampled values
    public:
	WeightedTraceMonitor(std::vector<Node const *> const &nodes, unsigned long statlength);
	void update(unsigned int chain) override;
	void value(std::vector<double> &v, unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
	virtual std::vector<double> stat(unsigned int chain) = 0;
	virtual std::vector<double> weight(unsigned int chain) = 0;
    };
    
}

#endif /* WEIGHTED_TRACE_MONITOR_H_ */
