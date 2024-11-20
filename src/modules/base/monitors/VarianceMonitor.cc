#include <config.h>
#include <graph/Node.h>

#include "VarianceMonitor.h"

using std::vector;

namespace jags {
    namespace base {

	ValueVarMonitor::ValueVarMonitor(NodeArraySubset const &subset)
	    : VarMonitor(subset.nodes(), subset.length()), _subset(subset)
	{
	}
    
	vector<unsigned long> ValueVarMonitor::dim() const
	{
	    return _subset.dim();
	}

	vector<double> ValueVarMonitor::stat(unsigned int ch)
	{
	    return _subset.value(ch);
	}

    }
}
