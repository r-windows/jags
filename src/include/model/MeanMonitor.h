#ifndef MEAN_MONITOR_H_
#define MEAN_MONITOR_H_

#include <model/Monitor.h>
#include <model/NodeArraySubset.h>

#include <vector>

namespace jags {

    /**
     * @short Stores running mean of a given stat
     */
    class MeanMonitor : public Monitor {
	std::vector<std::vector<double>> _sums;
    public:
	MeanMonitor(std::vector<Node const *> const &nodes, unsigned long statlength);
	void update(unsigned int chain) override;
	void value(std::vector<double> &v, unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
	virtual std::vector<double> stat(unsigned int chain) = 0;
    };

}

#endif /* MEAN_MONITOR_H_ */
