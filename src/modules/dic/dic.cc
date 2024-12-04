#include <module/Module.h>

#include "NodeDensityMonitorFactory.h"
#include "PenaltyMonitorFactory.h"

using std::vector;

namespace jags {
namespace dic {

    class DICModule: public Module {
    public:
	DICModule();
	~DICModule() override;
    };
    
    DICModule::DICModule() 
	: Module("dic")
    {
	// density-related monitors
	insert(new NodeDensityMonitorFactory);
	// penalty monitors
	insert(new PenaltyMonitorFactory);
    }
    
    DICModule::~DICModule() {
	
	vector<MonitorFactory*> const &mvec = monitorFactories();
	for (unsigned int i = 0; i < mvec.size(); ++i) {
	    delete mvec[i];
	}
    }

}}

jags::dic::DICModule _dic_module;

