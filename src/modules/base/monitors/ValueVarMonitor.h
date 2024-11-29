#ifndef VALUE_VAR_MONITOR_H_
#define VALUE_VAR_MONITOR_H_

#include <model/VarMonitor.h>

#include <vector>

namespace jags {

    class NodeArraySubset;
    
    namespace base {

	/**
	 * @short Stores running variance of a given Node
	 */
	class ValueVarMonitor : public VarMonitor {
	public:
	    ValueVarMonitor(NodeArraySubset const &subset);
	};
	
    }
}

#endif /* VALUE_VAR_MONITOR_H_ */
