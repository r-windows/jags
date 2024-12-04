#ifndef WAIC_MONITOR_H_
#define WAIC_MONITOR_H_

#include <model/VarMonitor.h>

#include <vector>

namespace jags {

    class StochasticNode;
    
    namespace dic {

	class WAICMonitor : public VarMonitor {
	public:
	    WAICMonitor(std::vector<StochasticNode const *> const &snodes);
	    ~WAICMonitor() override;
	};

    }
}

#endif /* WAIC_MONITOR_H_ */
