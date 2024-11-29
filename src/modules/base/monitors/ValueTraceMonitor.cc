#include <config.h>
#include <graph/Node.h>

#include "ValueTraceMonitor.h"
#include "ValueStat.h"

using std::vector;

namespace jags {
    namespace base {

	ValueTraceMonitor::ValueTraceMonitor(NodeArraySubset const &subset)
	    : TraceMonitor(subset.nodes(), new ValueStat(subset))
	{
	}

    }
}
