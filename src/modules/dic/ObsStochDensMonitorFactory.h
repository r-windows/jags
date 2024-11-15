#ifndef OBS_STOCH_DENS_MONITOR_FACTORY_H_
#define OBS_STOCH_DENS_MONITOR_FACTORY_H_

#include <model/MonitorFactory.h>
#include <model/Monitor.h>

namespace jags {
namespace dic {

    /**
     * @short Factory for creating density-related monitors for
     * observed stochastic nodes
    */
    class ObsStochDensMonitorFactory : public MonitorFactory
    {
      public:
	Monitor *getMonitor(std::string const &name, Range const &range,
			    BUGSModel *model, std::string const &stat,
			    std::string const &summary,
			    std::string &msg) override;
	std::string name() const override;
    };
    
}}

#endif /* OBS_STOCH_DENS_MONITOR_FACTORY_H_ */
