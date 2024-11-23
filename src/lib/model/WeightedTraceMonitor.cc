#include <config.h>
#include <graph/Node.h>

#include <algorithm>

#include <model/WeightedTraceMonitor.h>

using std::vector;
using std::string;

namespace jags {

    WeightedTraceMonitor::WeightedTraceMonitor(vector<Node const *> const &nodes, unsigned long statlength)
	: Monitor(nodes), _values(nchain()),
	  _weight_sums(nchain(), vector<double>(statlength, 0.0))
    {
    }
    
    void WeightedTraceMonitor::update(unsigned int ch)
    {
	vector<double> value = stat(ch);
	vector<double> wt = weight(ch);
	
	for (unsigned int i = 0; i < value.size(); ++i) {
	    _values[ch].push_back(wt[i] * value[i]);
	    _weight_sums[ch][i] += wt[i];
	}
    }

    void WeightedTraceMonitor::value(vector<double> &v, unsigned int ch) const
    {
	unsigned long n = niter();
	unsigned long m = nchain();

	vector<double> weight_means(m);
	for (unsigned int j = 0; j < m; ++j) {
	    weight_means[j] = _weight_sums[ch][j]/n;
	}
	unsigned long k = 0;
	for(unsigned long i = 0; i < n; ++i) {
	    for (unsigned long j = 0; j < m; ++j) {
		v[k] = _values[ch][k] / weight_means[j];
		++k;
	    }
	}
    }
    
    bool WeightedTraceMonitor::poolChains() const
    {
	return false;
    }

    bool WeightedTraceMonitor::poolIterations() const
    {
	return false;
    }

}
