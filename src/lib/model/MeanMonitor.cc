#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include <model/MeanMonitor.h>
#include <model/MonitorStat.h>

using std::vector;

namespace jags {

    MeanMonitor::MeanMonitor(vector<Node const *> const &nodes, MonitorStat *stat)
	: Monitor(nodes, stat),
	  _value_sums(nchain(), vector<double>(stat->length(), 0.0))
    {
	if (stat->weighted()) {
	    _weight_sums = vector<vector<double>>(nchain(), vector<double>(stat->length(), 0.0));	    
	}
    }

    MeanMonitor::~MeanMonitor()
    {
    }
    
    void MeanMonitor::update(unsigned int ch)
    {
	const vector<double> value = stat()->value(ch);
	if (stat()->weighted()) {
	    //Weighted 
	    const vector<double> wt = stat()->weight(ch);
	    for (unsigned int i = 0; i < value.size(); ++i) {
		_value_sums[ch][i] += wt[i] * value[i];
		_weight_sums[ch][i] += wt[i];
	    }
	}
	else {
	    //Unweighted
	    for (unsigned int i = 0; i < value.size(); ++i) {
		_value_sums[ch][i] += value[i];
	    }
	}
    }

    void MeanMonitor::value(vector<double> &v, unsigned int ch) const
    {
	copy(_value_sums[ch].begin(), _value_sums[ch].end(), v.begin());
	if (stat()->weighted()) {
	    //Weighted 
	    for (unsigned int i = 0; i < v.size(); ++i) {
		v[i] /= _weight_sums[ch][i];
	    }

	}
	else {
	    //Unweighted
	    unsigned long n = niter();
	    for (unsigned int i = 0; i < v.size(); ++i) {
		v[i] /= n;
	    }
	}
    }

    bool MeanMonitor::poolChains() const
    {
	return false;
    }
    
    bool MeanMonitor::poolIterations() const
    {
	return true;
    }

}
