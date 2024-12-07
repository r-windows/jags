#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include <model/MeanMonitor.h>
#include <model/MonitorStat.h>

using std::vector;

namespace jags {

    static unsigned long weight_size(MonitorStat *stat)
    {
	switch(stat->weighted()) {
	case UNWEIGHTED:
	    return 0;
	case SCALAR_WEIGHT:
	    return 1;
	case VECTOR_WEIGHT:
	    return stat->length();
	}
    }

    MeanMonitor::MeanMonitor(vector<Node const *> const &nodes, MonitorStat *stat)
	: Monitor(nodes, stat),
	  _S(nchain(), vector<double>(stat->length(), 0.0)),
	  _W(nchain(), vector<double>(weight_size(stat), 0.0))
    {
    }

    MeanMonitor::~MeanMonitor()
    {
    }
    
    void MeanMonitor::update(unsigned int ch)
    {
	vector<double> &S = _S[ch];
	vector<double> &W = _W[ch];
	
	const vector<double> value = stat()->value(ch);
	const vector<double> weight = stat()->weight(ch);
	
	switch(stat()->weighted()) {
	case UNWEIGHTED:
	    for (unsigned int i = 0; i < value.size(); ++i) {
		S[i] += value[i];
	    }
	    break;
	case SCALAR_WEIGHT:
	    for (unsigned int i = 0; i < value.size(); ++i) {
		S[i] += weight[0] * value[i];
	    }
	    W[0] += weight[0];
	    break;
	case VECTOR_WEIGHT:
	    for (unsigned int i = 0; i < value.size(); ++i) {
		S[i] += weight[i] * value[i];
		W[i] += weight[i];
	    }
	}
    }

    void MeanMonitor::value(vector<double> &v, unsigned int ch) const
    {
	copy(_S[ch].begin(), _S[ch].end(), v.begin());

	vector<double> const &W = _W[ch];
	unsigned long n = niter();
	    
	switch(stat()->weighted()) {
	case UNWEIGHTED:
	    for (unsigned int i = 0; i < v.size(); ++i) {
		v[i] /= n;
	    }
	    break;
	case SCALAR_WEIGHT:
	    for (unsigned int i = 0; i < v.size(); ++i) {
		v[i] /= W[0];
	    }
	    break;
	case VECTOR_WEIGHT:
	    for (unsigned int i = 0; i < v.size(); ++i) {
		v[i] /= W[i];
	    }
	    break;
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
