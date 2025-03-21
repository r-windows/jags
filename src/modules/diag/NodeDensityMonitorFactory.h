#ifndef NODE_DENSITY_MONITOR_FACTORY_H_
#define NODE_DENSITY_MONITOR_FACTORY_H_

#include <model/MonitorFactory.h>
#include <model/Monitor.h>

namespace jags {
    namespace diag {

	/**
	 * @short Factory for creating density-related monitors for a given node
	 */
	class NodeDensityMonitorFactory : public MonitorFactory
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

#endif /* NODE_DENSITY_MONITOR_FACTORY_H_ */
