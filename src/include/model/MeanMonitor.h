#ifndef MEAN_MONITOR_H_
#define MEAN_MONITOR_H_

#include <model/Monitor.h>

namespace jags {

    /**
     * @short Stores running mean of a given stat
     */
    class MeanMonitor : public Monitor {
	std::vector<std::vector<double>> _S;
	std::vector<std::vector<double>> _W;
	std::vector<bool> _missing;
    public:
	MeanMonitor(std::vector<Node const *> const &nodes, MonitorStat *stat);
	void update(unsigned int chain) override;
	std::vector<double> value(unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
    };

}

#endif /* MEAN_MONITOR_H_ */
