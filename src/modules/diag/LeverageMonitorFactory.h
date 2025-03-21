#ifndef LEVERAGE_MONITOR_FACTORY_H_
#define LEVERAGE_MONITOR_FACTORY_H_

#include <model/MonitorFactory.h>
#include <model/Monitor.h>

namespace jags {
    namespace diag {
	
	/**
	 * @short Factory for creating penalties for predictive information criteria
	 */
	class LeverageMonitorFactory : public MonitorFactory
	{
	public:
	    Monitor *getMonitor(std::string const &name, Range const &range,
				BUGSModel *model,
				std::string const &stat,
				std::string const &summary,
				std::string &msg) override;
	    std::string name() const override;
	};
	
    }
}

#endif /* LEVERAGE_MONITOR_FACTORY_H_ */
