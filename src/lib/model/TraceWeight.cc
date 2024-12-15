#include <config.h>

#include <model/TraceWeight.h>
#include <model/MonitorStat.h>
#include <util/nainf.h>

#include <stdexcept>

using std::vector;
using std::logic_error;

namespace jags {

    TraceWeight::TraceWeight(vector<Node const *> const &nodes,
			     MonitorStat *stat)
	: WeightMonitor(nodes, stat), _values(nchain())
    {
    }
    
    void TraceWeight::update(unsigned int chain)
    {
	const vector<double> v = _stat->weight(chain);
	_values[chain].insert(_values[chain].end(), v.begin(), v.end());
    }

    vector<double> TraceWeight::value(unsigned int chain) const
    {
	unsigned long m = length();
	unsigned long n = niter();

	vector<double> w = _values[chain];

	// Calculate sum of weights
	vector<double> W(m);
	for (unsigned long i = 0; i < n; ++i) {
	    for (unsigned long j = 0; j < m; ++j) {
		if (!_missing[j]) {
		    W[j] += w[m*i + j];
		}
	    }
	}
	
	// Normalize weights so that they have mean 1
	for (unsigned long i = 0; i < n; ++i) {
	    for (unsigned long j = 0; j < m; ++j) {
		if (_missing[j]) {
		    w[m*i + j] = JAGS_NA;
		}
		else {
		    w[m*i + j] *= n/W[j];
		}
	    }
	}

	return w;
    }

    bool TraceWeight::poolIterations() const
    {
	return false;
    }

}
