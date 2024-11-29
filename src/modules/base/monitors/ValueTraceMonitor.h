#ifndef VALUE_TRACE_MONITOR_H_
#define VALUE_TRACE_MONITOR_H_

#include <model/TraceMonitor.h>

namespace jags {

    class NodeArraySubset;
    
    namespace base {

	/**
	 * @short Stores sampled values of a given Node
	 */
	class ValueTraceMonitor : public TraceMonitor {
	public:
	    ValueTraceMonitor(NodeArraySubset const &subset);
	};
	
    }
}

#endif /* VALUE_TRACE_MONITOR_H_ */
