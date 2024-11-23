#ifndef WEIGHTED_MEAN_MONITOR_H_
#define WEIGHTED_MEAN_MONITOR_H_

#include <model/Monitor.h>
#include <model/NodeArraySubset.h>

#include <vector>

namespace jags {

    /**
     * @short Stores running mean of a given stat
     */
    class WeightedMeanMonitor : public Monitor {
	std::vector<std::vector<double>> _value_sums;
	std::vector<std::vector<double>> _weight_sums;
    public:
	WeightedMeanMonitor(std::vector<Node const *> const &nodes, unsigned long statlength);
	void update(unsigned int chain) override;
	void value(std::vector<double> &v, unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
	virtual std::vector<double> stat(unsigned int chain) = 0;
	virtual std::vector<double> weight(unsigned int chain) = 0;
    };

}

#endif /* WEIGHTED_MEAN_MONITOR_H_ */
