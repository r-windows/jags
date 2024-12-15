#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include <model/ESSWeight.h>
#include <model/MonitorStat.h>

using std::vector;

namespace jags {

    ESSWeight::ESSWeight(vector<Node const *> const &nodes, MonitorStat *stat)
	: WeightMonitor(nodes, stat),
	  _W(nchain(), vector<double>(length(), 0.0)),
	  _W2(nchain(), vector<double>(length(), 0.0))
    {
    }
    
    void ESSWeight::update(unsigned int ch)
    {
	vector<double> &W = _W[ch]; // Sum of weights
	vector<double> &WW = _W2[ch]; // Sum of squared weights
	
	const vector<double> weight = _stat->weight(ch);
	
	for (unsigned int i = 0; i < weight.size(); ++i) {
	    if (!_missing[i]) {
		W[i] += weight[i];
		WW[i] += weight[i] * weight[i];
	    }
	}
    }

    vector<double>  ESSWeight::value(unsigned int ch) const
    {
	vector<double> const &W = _W[ch];
	vector<double> const &WW = _W2[ch];

	unsigned long length = W.size();
	vector<double> ESS(length);
	for (unsigned int i = 0; i < length; ++i) {
	    if (_missing[i]) {
		ESS[i] = JAGS_NA;
	    }
	    else {
		ESS[i] = W[i]*W[i]/WW[i];
	    }
	}

	return ESS;
    }
    
    bool ESSWeight::poolIterations() const
    {
	return true;
    }

}
