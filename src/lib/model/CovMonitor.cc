#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

//#include <model/CovMonitor.h>
//#include <model/MonitorStat.h>

using std::vector;

namespace jags {
    
    CovMonitor::CovMonitor(vector<Node const *> const &nodes,
			   MonitorStat *stat)
	: Monitor(nodes), _stat(stat),
	  _S(nchain(), vector<double>(stat->length(), 0.0)),
	  _SS(nchain(), vector<double>(stat->length() * stat->length(), 0.0)),
	  _W(nchain(), 0.0),
	  _WW(nchain(), 0.0),
	  _missing(stat->length(), false)
    {
	if (stat->weighted() == VECTOR_WEIGHT) {
	    throw logic_error("Cannot construct CovMonitor with vector weights");
	}
    }
    
    CovMonitor::~CovMonitor()
    {
	delete _stat;
    }

    static void update_missing(vector<double> const &value, vector<bool> &missing)
    {
	for (unsigned long i = 0; i < value.size(); ++i) {
	    if (!missing[i] && jags_isna(value[i]))
		missing[i] = true;
	}
    }

    static void update_value(vector<double> const &value, vector<bool> const &missing, double wt, double W, vector<double> &S, vector<double> &SS)
    {
	unsigned long p = value.size();
	
	for (unsigned long i = 0; i < p; ++i) {
	    if (missing[i]) continue;
	    if (W > 0) {
		double delta_i = value[i] - S[i]/W;
		for (unsigned long j = 0; j < p; ++j) {
		    if (missing[j]) continue;
		    double delta_j = value[j] - S[j]/W;
		    SS[p*i + j] += wt * delta_i * delta_j * W / (W + wt);
		}
	    }
	    S[i] += wt * value[i];
	}
    }
    
    static void update_weight(double wt, double &W, double &WW) {
	W += wt;
	WW += wt * wt;
    }

    static double denominator(double W, double WW) {
	return W - WW/W;
    }
    
    void CovMonitor::update(unsigned int chain)
    {
	vector<double> &S = _S[chain];   // sum of values
	vector<double> &SS = _SS[chain]; // sum of squares of residuals	
	vector &W = _W[chain];   // sum of weights
	vector &WW = _WW[chain]; // sum of squares of weights
	
	vector<double> value = stat()->value(chain);
	vector<double> weight = stat()->weight(chain);
	update_missing(value, _missing);
	
	unsigned long n = niter();
	switch(stat()->weighted()) {
	case UNWEIGHTED:
	    update_value(value, 1, n - 1, S, SS);
	    break;
	case SCALAR_WEIGHT:
	    update_value(value, weight[0], W, S, SS);
	    update_weight(weight[0], W, WW);
	    break;
	case VECTOR_WEIGHT:
	    break;
	}
    }

    vector<double> CovMonitor::value(unsigned int ch) const
    {
	vector<double> V = _SS[ch];
	
	double d = 0;
	unsigned long n = niter();
	
	switch(stat()->weighted()) {
	case UNWEIGHTED:
	    d = denominator(n, n);
	    break;
	case SCALAR_WEIGHT:
	    d = denominator(_W[ch], _WW[ch]);
	    break;
	case VECTOR_WEIGHT:
	    break;
	}

	unsigned long m = _stat->length();
	for (unsigned long i = 0; i < m; ++i) {
	    for (unsigned long j = 0; j < m; ++j) {
		if (_missing[j] || _missing[j]) {
		    V[p*i + j] = JAGS_NA;
		}
		else {
		    V[p*i + j] /= d;
		}
	    }
	}

	return v;
    }

    unsigned long CovMonitor::length() const
    {
	unsigned long = _stat->length();
	return long * long;
    }
    
    vector<unsigned long> CovMonitor::dim() const
    {
	vector<unsigned long> d = _stat->dim();
	vector<unsigned long> dim = d;
	append(dim.end(), d.begin(), d.end());
	return dim;
    }


    bool CovMonitor::poolChains() const
    {
	return false;
    }

    bool CovMonitor::poolIterations() const
    {
	return true;
    }

}
