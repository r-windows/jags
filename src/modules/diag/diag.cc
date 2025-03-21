#include <module/Module.h>

#include "NodeDensityMonitorFactory.h"
#include "LeverageMonitorFactory.h"

using std::vector;

namespace jags {
    namespace diag {

	class DiagModule: public Module {
	public:
	    DiagModule();
	    ~DiagModule() override;
	};
    
	DiagModule::DiagModule() 
	    : Module("diag")
	{
	    // density-related monitors
	    insert(new NodeDensityMonitorFactory);
	    // leverage monitors
	    insert(new LeverageMonitorFactory);
	}
    
	DiagModule::~DiagModule() {
	
	    vector<MonitorFactory*> const &mvec = monitorFactories();
	    for (unsigned int i = 0; i < mvec.size(); ++i) {
		delete mvec[i];
	    }
	}

    }
}

jags::diag::DiagModule _diag_module;

