#ifndef COV_MONITOR_H_
#define COV_MONITOR_H_

#include <model/Monitor.h>

namespace jags {

    class MonitorStat;
    
    /**
     * @short Stores running variance of a given stat
     */
    class CovMonitor : public Monitor {
	std::vector<std::vector<double>> _S, _SS;
	std::vector<double> _W, _WW;
	std::vector<bool> _missing;
    public:
	CovMonitor(std::vector<Node const *> const &nodes,
		   MonitorStat *stat);
	void update(unsigned int chain) override;
	std::vector<double> value(unsigned int chain) const override;
	std::vector<unsigned long> dim() const override;
	unsigned long length() const override;
	bool poolChains() const override;
	bool poolIterations() const override;
	std::vector<std::string> elementNames() const override;
    };
    
}

#endif /* COV_MONITOR_H_ */
