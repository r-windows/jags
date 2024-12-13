#include <config.h>
#include <graph/Node.h>

#include "ValueStat.h"

using std::vector;

namespace jags {
    namespace base {

	ValueStat::ValueStat(NodeArraySubset const &subset)
	    : MonitorStat(subset.dim(), subset.missing()), _subset(subset)
	{
	}

	vector<double> ValueStat::value(unsigned int ch) const
	{
	    return _subset.value(ch);
	}

    }
}
