#ifndef TRACE_MONITOR_H_
#define TRACE_MONITOR_H_

#include <model/Monitor.h>

namespace jags {

    class MonitorStat;
    
    /**
     * @short Stores sampled values for a given stat
     */
    class TraceMonitor : public Monitor {
	std::vector<std::vector<double>> _values; // sampled values
    public:
	TraceMonitor(std::vector<Node const *> const &nodes, MonitorStat *stat);
	~TraceMonitor();
	void update(unsigned int chain) override;
	std::vector<double> value(unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
    };
    
}

#endif /* TRACE_MONITOR_H_ */
