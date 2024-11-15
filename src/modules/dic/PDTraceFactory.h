#ifndef PD_TRACE_FACTORY_H_
#define PD_TRACE_FACTORY_H_

#include <model/MonitorFactory.h>

namespace jags {
namespace dic {

    class PDTraceFactory : public MonitorFactory
    {
      public:
	Monitor *getMonitor(std::string const &name, Range const &range,
			    BUGSModel *model, std::string const &stat,
			    std::string const &summary,
			    std::string &msg) override;
	std::string name() const override;
    };
    
}}

#endif /* PD_TRACE_FACTORY_H_ */
