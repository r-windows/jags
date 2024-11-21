#include <config.h>
#include <graph/Node.h>

#include "ValueTraceMonitor.h"

using std::vector;

namespace jags {
    namespace base {

	ValueTraceMonitor::ValueTraceMonitor(NodeArraySubset const &subset)
	    : TraceMonitor(subset.nodes()), _subset(subset)
	{
	}
    
	vector<unsigned long> ValueTraceMonitor::dim() const
	{
	    return _subset.dim();
	}

	vector<double> ValueTraceMonitor::stat(unsigned int ch)
	{
	    return _subset.value(ch);
	}

    }
}
