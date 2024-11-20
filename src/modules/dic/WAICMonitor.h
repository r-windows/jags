#ifndef WAIC_MONITOR_H_
#define WAIC_MONITOR_H_

#include <model/Monitor.h>

#include <vector>

namespace jags {

    class StochasticNode;
    struct RNG;
    
    namespace dic {

	class WAICMonitor : public Monitor {
	    std::vector<StochasticNode const *> _snodes;
	    std::vector<std::vector<double> > _mlik;
	    std::vector<std::vector<double> > _vlik;
	public:
	    WAICMonitor(std::vector<StochasticNode const *> const &snodes);
	    ~WAICMonitor() override;
	    std::vector<unsigned long> dim() const override;
	    void value(std::vector<double> &v, unsigned int chain) const override;
	    bool poolChains() const override;
	    bool poolIterations() const override;
	    void update(unsigned int chain) override;
	};

    }
}

#endif /* WAIC_MONITOR_H_ */
