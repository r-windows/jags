#include <config.h>
#include <graph/Node.h>

#include "MeanMonitor.h"

using std::vector;

namespace jags {
    namespace base {

	ValueMeanMonitor::ValueMeanMonitor(NodeArraySubset const &subset)
	    : MeanMonitor(subset.nodes(), subset.length()), _subset(subset)
	{
	}
	
	vector<unsigned long> ValueMeanMonitor::dim() const
	{
	    return _subset.dim();
	}

	vector<double> ValueMeanMonitor::stat(unsigned int ch)
	{
	    return _subset.value(ch);
	}
	

    }
}
