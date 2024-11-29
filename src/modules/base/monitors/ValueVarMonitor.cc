#include <config.h>

#include <model/NodeArraySubset.h>

#include "ValueVarMonitor.h"
#include "ValueStat.h"

using std::vector;

namespace jags {
    namespace base {

	ValueVarMonitor::ValueVarMonitor(NodeArraySubset const &subset)
	    : VarMonitor(subset.nodes(), new ValueStat(subset))
	{
	}

    }
}
