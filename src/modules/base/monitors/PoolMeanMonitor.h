#ifndef POOLMEAN_MONITOR_H_
#define POOLMEAN_MONITOR_H_

#include <model/Monitor.h>
#include <model/NodeArraySubset.h>

#include <vector>

namespace jags {
namespace base {

    /**
     * @short Stores running mean (pooled between chains) of a given Node
     */
    class PoolMeanMonitor : public Monitor {
	NodeArraySubset _subset;
	std::vector<double> _sums;
    public:
	PoolMeanMonitor(NodeArraySubset const &subset);
	void update(unsigned int chain) override;
	void value(std::vector<double> &v, unsigned int chain) const override;
	std::vector<unsigned long> dim() const override;
	bool poolChains() const override;
	bool poolIterations() const override;
    };

}}

#endif /* POOLMEAN_MONITOR_H_ */
