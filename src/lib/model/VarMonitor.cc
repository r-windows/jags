#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include <model/VarMonitor.h>
#include <model/MonitorStat.h>

using std::vector;

namespace jags {

    VarMonitor::VarMonitor(vector<Node const *> const &nodes,
			   MonitorStat *stat)
	: Monitor(nodes, stat),
	  _S(nchain(), vector<double>(stat->length(), 0.0)),
	  _SS(nchain(), vector<double>(stat->length(), 0.0))
    {
	if (stat->weighted()) {
	    _W = vector<vector<double>>(nchain(), vector<double>(stat->length(), 0.0));
	    _D = vector<vector<double>>(nchain(), vector<double>(stat->length(), 0.0));
	}
    }

    VarMonitor::~VarMonitor()
    {
    }
    
    void VarMonitor::update(unsigned int chain)
    {
	vector<double> value = stat()->value(chain);
	vector<double> &S = _S[chain]; // sum of values
	vector<double> &SS = _SS[chain]; // sum of squares of residuals	

	unsigned long n = niter();
	if (stat()->weighted()) {
	    //Weighted
	    vector<double> weight = stat()->weight(chain);
	    vector<double> &W = _W[chain]; // sum of weights
	    vector<double> &D = _D[chain]; // denominator
	    
	    for (unsigned int i = 0; i < value.size(); ++i) {
		double shrink = W[i] / (W[i] + weight[i]);
		if (n > 1) {
		    double delta = value[i] - S[i]/W[i];
		    SS[i] += weight[i] * delta * delta * shrink;
		}
		S[i] += weight[i] * value[i];
		W[i] += weight[i];
		/* Recursively defined expression for D = W - W2/W
		   where W is the sum of weights and W2 is the sum
		   of squares of the weights */
		D[i] += (2 * weight[i] + D[i]) * shrink;
	    }
	}
	else {
	    //Unweighted
	    double shrink = static_cast<double>(n-1)/n;
	    for (unsigned int i = 0; i < value.size(); ++i) {
		if (n > 1) {
		    double delta = value[i] - S[i]/(n-1);
		    SS[i] += delta * delta * shrink;
		}
		S[i] += value[i];
	    }
	}
    }

    void VarMonitor::value(vector<double> &v, unsigned int chain) const
    {
	unsigned long n = niter();
	copy(_SS[chain].begin(), _SS[chain].end(), v.begin());
	if (stat()->weighted()) {
	    // Weighted: separate denominator for each element
	    vector<double> const &D = _D[chain];
	    for (unsigned int i = 0; i < v.size(); ++i) {
		v[i] /= D[i];
	    }
	}
	else {
	    // Unweighted: common denominator based on sample size
	    double d = niter() - 1;
	    for (unsigned int i = 0; i < v.size(); ++i) {
		v[i] /= d;
	    }
	}
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
