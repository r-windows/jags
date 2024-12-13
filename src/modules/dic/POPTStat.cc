#include <config.h>

#include "POPTStat.h"

#include <graph/Node.h>

#include <cmath>

using std::vector;
using std::exp;

namespace jags {
    namespace dic {

	POPTStat::POPTStat(vector<Node const *> const &nodes,
				 vector<RNG *> const &rngs,
				 unsigned int nrep)
	    : MonitorStat(nodes.size()), _nodes(nodes), _rngs(rngs), _nrep(nrep)
	{
	}

	vector<double> POPTStat::value(unsigned int ch) const
	{
	    unsigned long n = _nodes.size();
	    unsigned long m = _nodes[0]->nchain();
	    
	    vector<double> v(n);
	    for (unsigned int k = 0; k < n; ++k) {
		double pdsum = 0;
		double wsum = 0;
		for (unsigned int j = 0; j < m; ++j) {
		    if (j != ch) {
			double wj = exp(- _nodes[k]->logDensity(j, PDF_FULL));
			pdsum += wj * _nodes[k]->KL(ch, j, _rngs[ch], _nrep);
			wsum += wj;
		    }
		}
		v[k] = pdsum / wsum;
	    }
	    return v;
	}

	WeightType POPTStat::weighted() const
	{
	    return VECTOR_WEIGHT;
	}
	
	vector<double> POPTStat::weight(unsigned int ch) const
	{
	    unsigned long n = _nodes.size();
	    unsigned long m = _nodes[0]->nchain();
	    
	    vector<double> w(n, 0.0);
	    for (unsigned int k = 0; k < n; ++k) {
		for (unsigned int j = 0; j < m; ++j) {
		    if (j != ch) {
			w[k] += exp(-_nodes[k]->logDensity(j, PDF_FULL));
		    }
		}
		w[k] *= exp(-_nodes[k]->logDensity(ch, PDF_FULL));
	    }
	    return w;
	}
	
    }
}
