#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include <model/WeightedMeanMonitor.h>

using std::vector;
using std::string;

namespace jags {

    WeightedMeanMonitor::WeightedMeanMonitor(vector<Node const *> const &nodes,
					     unsigned long statlength)
	: Monitor(nodes),
	  _value_sums(nchain(), vector<double>(statlength, 0.0)),
	  _weight_sums(nchain(), vector<double>(statlength, 0.0))
    {
    }
    
    void WeightedMeanMonitor::update(unsigned int ch)
    {
	vector<double> value = stat(ch);
	vector<double> wt = weight(ch);
	
	for (unsigned int i = 0; i < value.size(); ++i) {
	    _value_sums[ch][i] += wt[i] * value[i];
	    _weight_sums[ch][i] += wt[i];
	}
    }
    
    void WeightedMeanMonitor::value(vector<double> &v, unsigned int ch) const
    {
	for (unsigned int i = 0; i < v.size(); ++i) {
	    v[i] = _value_sums[ch][i]/_weight_sums[ch][i];
	}
    }
    
    bool WeightedMeanMonitor::poolChains() const
    {
	return false;
    }
    
    bool WeightedMeanMonitor::poolIterations() const
    {
	return true;
    }
    
}
