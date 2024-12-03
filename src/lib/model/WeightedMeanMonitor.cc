#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include <model/WeightedMeanMonitor.h>
#include <model/MonitorStat.h>

using std::vector;
using std::string;

namespace jags {

    WeightedMeanMonitor::WeightedMeanMonitor(vector<Node const *> const &nodes,
					     MonitorStat *stat)
	: Monitor(nodes, stat),
	  _value_sums(nchain(), vector<double>(stat->length(), 0.0)),
	  _weight_sums(nchain(), vector<double>(stat->length(), 0.0))
    {
    }
    
    void WeightedMeanMonitor::update(unsigned int ch)
    {
	vector<double> value = stat()->value(ch);
	vector<double> wt = stat()->weight(ch);
	
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
