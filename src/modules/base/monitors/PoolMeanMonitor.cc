#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include "PoolMeanMonitor.h"

using std::vector;
using std::string;

namespace jags {
namespace base {

    PoolMeanMonitor::PoolMeanMonitor(NodeArraySubset const &subset)
	: Monitor(subset.nodes()), _subset(subset), _sums(subset.length(), 0.0)
    {
    }
    
    void PoolMeanMonitor::update(unsigned int)
    {
	unsigned long m = nchain();
	
	for (unsigned int ch = 0; ch < m; ++ch) {
	    vector<double> value = _subset.value(ch);
	    for (unsigned int i = 0; i < value.size(); ++i) {
		if (jags_isna(value[i])) {
		    _sums[i] = JAGS_NA;
		}
		else {
		    _sums[i] += value[i];
		}
	    }
	}
    }

    void PoolMeanMonitor::value(vector<double> &v, unsigned int) const
    {
	unsigned long n = nchain();
	copy(_sums.begin(), _sums.end(), v.begin());
	for (unsigned int i = 0; i < v.size(); ++i) {
	    v[i] /= n;
	}
    }

    vector<unsigned long> PoolMeanMonitor::dim() const
    {
	return _subset.dim();
    }

    bool PoolMeanMonitor::poolChains() const
    {
	return true;
    }

    bool PoolMeanMonitor::poolIterations() const
    {
	return true;
    }

}}
