#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include <model/VarMonitor.h>
#include <model/MonitorStat.h>

using std::vector;

namespace jags {

    static unsigned long weight_size(MonitorStat const *stat)
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

    VarMonitor::VarMonitor(vector<Node const *> const &nodes,
			   MonitorStat *stat)
	: Monitor(nodes), _stat(stat),
	  _S(nchain(), vector<double>(stat->length(), 0.0)),
	  _SS(nchain(), vector<double>(stat->length(), 0.0)),
	  _W(nchain(), vector<double>(weight_size(stat), 0.0)),
	  _WW(nchain(), vector<double>(weight_size(stat), 0.0)),
	  _missing(stat->missing())
    {
    }

    VarMonitor::~VarMonitor()
    {
	delete _stat;
    }

    static void update_value(double value, double wt, double W, double &S, double &SS) {
	if (W > 0) {
	    double delta = value - S/W;
	    SS += wt * delta * delta * W / (W + wt);
	}
	S += wt * value;
    }

    static void update_weight(double wt, double &W, double &WW) {
	W += wt;
	WW += wt * wt;
    }

    static inline double denominator(double W, double WW) {
	/*
	  Denominator for weighted sum of squares with
	  degree-of-freedom adjustment for estimating the weighted
	  mean. This could be expressed as W*(1 - 1/ESS) where ESS is
	  the effective sample size of the weights
	*/
	return W - WW/W;
    }
    
    void VarMonitor::update(unsigned int chain)
    {
	vector<double> &S = _S[chain];   // sum of values
	vector<double> &SS = _SS[chain]; // sum of squares of residuals	
	vector<double> &W = _W[chain];   // sum of weights
	vector<double> &WW = _WW[chain]; // sum of squares of weights
	
	const vector<double> value = _stat->value(chain);
	const vector<double> weight = _stat->weight(chain);

	unsigned long n = niter();
	for (unsigned long i = 0; i < value.size(); ++i) {
	    if (_missing[i]) continue;
	    switch(_stat->weighted()) {
	    case UNWEIGHTED:
		update_value(value[i], 1, n - 1, S[i], SS[i]);
		break;
	    case SCALAR_WEIGHT:
		update_value(value[i], weight[0], W[0], S[i], SS[i]);
		break;
	    case VECTOR_WEIGHT:
		update_value(value[i], weight[i], W[i], S[i], SS[i]);
		break;
	    }
	}

	switch(_stat->weighted()) {
	case UNWEIGHTED:
	    break;
	case SCALAR_WEIGHT:
	    update_weight(weight[0], W[0], WW[0]);
	    break;
	case VECTOR_WEIGHT:
	    for (unsigned int i = 0; i < value.size(); ++i) {
		if (!_missing[i]) {
		    update_weight(weight[i], W[i], WW[i]);
		}
	    }
	    break;
	}
	
    }

    vector<double> VarMonitor::value(unsigned int ch) const
    {
	vector<double> v = _SS[ch];
	unsigned long n = niter();

	for (unsigned long i = 0; i < v.size(); ++i) {
	    if (_missing[i]) {
		v[i] = JAGS_NA;
	    }
	    else {
		switch(_stat->weighted()) {
		case UNWEIGHTED:
		    v[i] /= denominator(n, n);
		    break;
		case SCALAR_WEIGHT:
		    v[i] /= denominator(_W[ch][0], _W[ch][0]);
		    break;
		case VECTOR_WEIGHT:
		    v[i] /= denominator(_W[ch][i], _WW[ch][i]);
		    break;
		}
	    }
	}

	return v;
    }
    
    unsigned long VarMonitor::length() const
    {
	return _stat->length();
    }
    
    vector<unsigned long> VarMonitor::dim() const
    {
	return _stat->dim();
    }

    bool VarMonitor::poolChains() const
    {
	return false;
    }

    bool VarMonitor::poolIterations() const
    {
	return true;
    }

}
