#ifndef VALUE_TRACE_MONITOR_H_
#define VALUE_TRACE_MONITOR_H_

#include <model/TraceMonitor.h>
#include <model/NodeArraySubset.h>

#include <vector>

namespace jags {
    namespace base {

	/**
	 * @short Stores sampled values of a given Node
	 */
	class ValueTraceMonitor : public TraceMonitor {
	    NodeArraySubset _subset;
	public:
	    ValueTraceMonitor(NodeArraySubset const &subset);
	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int chain) override;
	};
	
    }
}

#endif /* VALUE_TRACE_MONITOR_H_ */
