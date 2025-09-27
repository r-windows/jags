#ifndef COV_MONITOR_H_
#define COV_MONITOR_H_

#include <model/Monitor.h>

namespace jags {

    class MonitorStat;
    
    /**
     * @short Stores running variance-covariance matrix of a given stat
     *
     * Unlike most other Monitor subclasses, CovMonitor does not
     * preserve the dimensions of the stat. Instead, the stat is
     * vectorized and the monitored value is always a square symmetric
     * matrix, regardless of the dimensions of the stat. 
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
	/**
	 * Overriding the default implementation because CovMonitor
	 * vectorises the stat. The unit monitored value is an LxL
	 * matrix where L is the length of the stat.
	 */
	std::vector<unsigned long> dim() const override;
	/**
	 * Overriding the default implementation because CovMonitor
	 * vectorises the stat. The length of the unit monitored value
	 * L^2 where L is the length of the stat.
	 */
	unsigned long length() const override;
	bool poolChains() const override;
	bool poolIterations() const override;
	std::vector<std::string> elementNames() const override;
    };
    
}

#endif /* COV_MONITOR_H_ */
