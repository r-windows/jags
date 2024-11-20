#ifndef TRACE_MONITOR_H_
#define TRACE_MONITOR_H_

#include <model/Monitor.h>
#include <model/NodeArraySubset.h>

#include <vector>

namespace jags {
    
    /**
     * @short Stores sampled values for a given stat
     */
    class TraceMonitor : public Monitor {
	std::vector<std::vector<double>> _values; // sampled values
    public:
	TraceMonitor(std::vector<Node const *> const &nodes);
	void update(unsigned int chain) override;
	void value(std::vector<double> &v, unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
	virtual std::vector<double> stat(unsigned int chain) = 0;
    };
    
}

#endif /* TRACE_MONITOR_H_ */
