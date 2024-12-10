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
	unsigned long s = 0;
	switch(stat->weighted()) {
	case UNWEIGHTED:
	    break;
	case SCALAR_WEIGHT:
	    s = 1;
	    break;
	case VECTOR_WEIGHT:
	    s = stat->length();
	    break;
	}
	return s;
    }

    MeanMonitor::MeanMonitor(vector<Node const *> const &nodes, MonitorStat *stat)
	: Monitor(nodes, stat),
	  _S(nchain(), vector<double>(stat->length(), 0.0)),
	  _W(nchain(), vector<double>(weight_size(stat), 0.0)),
	  _missing(stat->length(), false)
    {
    }

    MeanMonitor::~MeanMonitor()
    {
    }
    
    void MeanMonitor::update(unsigned int ch)
    {
	vector<double> &S = _S[ch]; // Sum of (weighted) values
	vector<double> &W = _W[ch]; // Sum of weights
	
	const vector<double> value = stat()->value(ch);
	const vector<double> weight = stat()->weight(ch);
	
	for (unsigned int i = 0; i < value.size(); ++i) {
	    if (_missing[i]) {
		continue;
	    }
	    if (jags_isna(value[i])) {
		_missing[i] = true;
		continue;
	    }
	    switch(stat()->weighted()) {
	    case UNWEIGHTED:
		S[i] += value[i];
		break;
	    case SCALAR_WEIGHT:
		S[i] += weight[0] * value[i];
		break;
	    case VECTOR_WEIGHT:
		S[i] += weight[i] * value[i];
		break;
	    }
	}
	
	switch(stat()->weighted()) {
	case UNWEIGHTED:
	    break;
	case SCALAR_WEIGHT:
	    W[0] += weight[0];
	    break;
	case VECTOR_WEIGHT:
	    for (unsigned int i = 0; i < value.size(); ++i) {
		if (!_missing[i]) W[i] += weight[i];
	    }
	    break;
	}
    }

    vector<double>  MeanMonitor::value(unsigned int ch) const
    {
	vector<double> v = _S[ch];
	vector<double> const &W = _W[ch];
	unsigned long n = niter();

	for (unsigned int i = 0; i < v.size(); ++i) {
	    if (_missing[i]) {
		v[i] = JAGS_NA;
	    }
	    else {
		switch(stat()->weighted()) {
		case UNWEIGHTED:
		    v[i] /= n;
		    break;
		case SCALAR_WEIGHT:
		    v[i] /= W[0];
		    break;
		case VECTOR_WEIGHT:
		    v[i] /= W[i];
		    break;
		}
	    }
	}

	return v;
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
