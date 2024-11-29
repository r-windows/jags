#include <config.h>
#include <graph/Node.h>

#include "ValueStat.h"

using std::vector;

namespace jags {
    namespace base {

	ValueStat::ValueStat(NodeArraySubset const &subset)
	    : MonitorStat(), _subset(subset)
	{
	}

	unsigned long ValueStat::length() const
	{
	    return _subset.length();
	}
	
	vector<unsigned long> ValueStat::dim() const
	{
	    return _subset.dim();
	}

	vector<double> ValueStat::value(unsigned int ch) const
	{
	    return _subset.value(ch);
	}

    }
}
