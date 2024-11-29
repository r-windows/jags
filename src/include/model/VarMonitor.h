#ifndef VAR_MONITOR_H_
#define VAR_MONITOR_H_

#include <model/Monitor.h>

#include <vector>

namespace jags {

    class MonitorStat;
    
    /**
     * @short Stores running variance of a given stat
     */
    class VarMonitor : public Monitor {
	MonitorStat const *_stat;
	std::vector<std::vector<double>> _sums;
	std::vector<std::vector<double>> _sum_of_squares;
    public:
	VarMonitor(std::vector<Node const *> const &nodes,
		   MonitorStat const *stat);
	~VarMonitor();
	void update(unsigned int chain) override;
	void value(std::vector<double> &v, unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
	std::vector<unsigned long> dim() const override;
    };

}

#endif /* VAR_MONITOR_H_ */
