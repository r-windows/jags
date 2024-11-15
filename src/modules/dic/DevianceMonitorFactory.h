#ifndef DEVIANCE_MONITOR_FACTORY_H_
#define DEVIANCE_MONITOR_FACTORY_H_

#include <model/MonitorFactory.h>

namespace jags {
namespace dic {
	
	// DevianceMonitorFactory (and DevianceTrace/DevianceMean) could now be retired?
	// The same quantities are provided by ObsStochDensMonitorFactory and are named for backwards-compatibility
    class DevianceMonitorFactory : public MonitorFactory
    {
      public:
	Monitor *getMonitor(std::string const &name, Range const &range,
			    BUGSModel *model, std::string const &stat,
			    std::string const &summary,
			    std::string &msg) override;
	std::string name() const override;
    };
    
}}

#endif /* DEVIANCE_MONITOR_FACTORY_H_ */
