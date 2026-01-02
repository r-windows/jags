#include <module/Module.h>

namespace jags {
    namespace dic {

	class DICModule: public Module {
	public:
	    DICModule();
	};
    
	DICModule::DICModule() 
	    : Module("dic")
	{
	}
 
    }
}

jags::dic::DICModule _dic_module;

