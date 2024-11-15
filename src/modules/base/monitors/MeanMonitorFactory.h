#ifndef MEAN_MONITOR_FACTORY_H_
#define MEAN_MONITOR_FACTORY_H_

#include <model/MonitorFactory.h>

namespace jags {
namespace base {

    class MeanMonitorFactory : public MonitorFactory
    {
      public:
	Monitor *getMonitor(std::string const &name, Range const &range, 
			    BUGSModel *model,
			    std::string const &stat,
			    std::string const &summary,
			    std::string &msg) override;
	std::string name() const override;
    };
    
}}

#endif /* MEAN_MONITOR_FACTORY_H_ */
