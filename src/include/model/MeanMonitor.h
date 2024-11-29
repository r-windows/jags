#ifndef MEAN_MONITOR_H_
#define MEAN_MONITOR_H_

#include <model/Monitor.h>
#include <model/MonitorStat.h>

namespace jags {

    /**
     * @short Stores running mean of a given stat
     */
    class MeanMonitor : public Monitor {
	MonitorStat const * _stat;
	std::vector<std::vector<double>> _sums;
    public:
	MeanMonitor(std::vector<Node const *> const &nodes, MonitorStat const *stat);
	~MeanMonitor();
	void update(unsigned int chain) override;
	void value(std::vector<double> &v, unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
	std::vector<unsigned long> dim() const override;
    };

}

#endif /* MEAN_MONITOR_H_ */
