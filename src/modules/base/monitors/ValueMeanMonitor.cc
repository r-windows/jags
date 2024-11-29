#include <config.h>

#include <model/NodeArraySubset.h>

#include "ValueMeanMonitor.h"
#include "ValueStat.h"

using std::vector;

namespace jags {
    namespace base {

	ValueMeanMonitor::ValueMeanMonitor(NodeArraySubset const &subset)
	    : MeanMonitor(subset.nodes(), new ValueStat(subset))
	{
	}

    }
}
