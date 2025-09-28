#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <model/CovMonitor.h>
#include <model/MonitorStat.h>

#include <stdexcept>

using std::vector;
using std::logic_error;
using std::string;

namespace jags {

    CovMonitor::CovMonitor(vector<Node const *> const &nodes,
			   MonitorStat *stat)
	: Monitor(nodes, stat),
	  _S(nchain(), vector<double>(stat->length(), 0.0)),
	  _SS(nchain(), vector<double>(stat->length() * stat->length(), 0.0)),
	  _W(nchain(), 0.0),
	  _WW(nchain(), 0.0),
	  _missing(stat->missing())
    {
	if (stat->weighted() == VECTOR_WEIGHT) {
	    throw logic_error("Cannot construct CovMonitor with vector weights");
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
	}

	for (unsigned long i = 0; i < p; ++i) {
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
	vector<double> &S  =  _S[chain]; // sum of values
	vector<double> &SS = _SS[chain]; // sum of squares of residuals	
	
	const vector<double> value = _stat->value(chain);
	const vector<double> weight = _stat->weight(chain);
	
	unsigned long n = niter();
	switch(_stat->weighted()) {
	case UNWEIGHTED:
	    update_value(value, _stat->missing(), 1, n - 1, S, SS);
	    break;
	case SCALAR_WEIGHT:
	    update_value(value, _stat->missing(), weight[0], _W[chain], S, SS);
	    update_weight(weight[0], _W[chain], _WW[chain]);
	    break;
	case VECTOR_WEIGHT:
	    break; //-Wswitch
	}
    }

    vector<double> CovMonitor::value(unsigned int ch) const
    {
	vector<double> V = _SS[ch];
	
	double d = 0;
	unsigned long n = niter();
	
	switch(_stat->weighted()) {
	case UNWEIGHTED:
	    d = denominator(n, n);
	    break;
	case SCALAR_WEIGHT:
	    d = denominator(_W[ch], _WW[ch]);
	    break;
	case VECTOR_WEIGHT:
	    break; //-Wswitch
	}

	unsigned long m = _stat->length();
	for (unsigned long i = 0; i < m; ++i) {
	    for (unsigned long j = 0; j < m; ++j) {
		if (_missing[i] || _missing[j]) {
		    V[m*i + j] = JAGS_NA;
		}
		else {
		    V[m*i + j] /= d;
		}
	    }
	}

	return V;
    }

    unsigned long CovMonitor::length() const
    {
	unsigned long length = _stat->length();
	return length  * length;
    }
    
    vector<unsigned long> CovMonitor::dim() const
    {
	return vector<unsigned long>(2, _stat->length());
    }

    bool CovMonitor::poolChains() const
    {
	return false;
    }

    bool CovMonitor::poolIterations() const
    {
	return true;
    }

    vector<string> CovMonitor::elementNames() const
    {
	vector<string> elt_names;

	vector<string> const &statnames = _stat->names();
	for (unsigned long i = 0; i < statnames.size(); ++i) {
	    for (unsigned long j = 0; j < statnames.size(); ++j) {
		elt_names.push_back(statnames[i] + ":" + statnames[j]);
	    }
	}

	return elt_names;
    }

    vector<vector<string>> CovMonitor::dimNames() const
    {
	return vector<vector<string>>(2, _stat->names());
    }
}
