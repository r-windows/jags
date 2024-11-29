#include <config.h>

#include "PenaltyPOPT.h"
#include <graph/Node.h>

#include <cmath>

using std::vector;
using std::string;

namespace jags {
    namespace dic {

	PenaltyPOPT::PenaltyPOPT(vector<Node const *> const &nodes,
				 vector<RNG *> const &rngs,
				 unsigned int nrep)
	    : WeightedMeanMonitor(nodes, nodes.size()), _rngs(rngs), _nrep(nrep)
	{
	}

	vector<unsigned long> PenaltyPOPT::dim() const
	{
	    return vector<unsigned long>(1, nodes().size());
	}

	vector<double> PenaltyPOPT::stat(unsigned int ch)
	{
	    vector<Node const *> const nodes = this->nodes();
	    unsigned long n = nodes.size();
	    unsigned long m = nchain();
	    
	    vector<double> v(n);
	    for (unsigned int k = 0; k < n; ++k) {
		double pdsum = 0;
		double wsum = 0;
		for (unsigned int j = 0; j < m; ++j) {
		    if (j != ch) {
			double wj = exp(- nodes[k]->logDensity(j, PDF_FULL));
			pdsum += wj * nodes[k]->KL(ch, j, _rngs[ch], _nrep);
			wsum += wj;
		    }
		}
		v[k] = pdsum / wsum;
	    }
	    return v;
	}

	vector<double> PenaltyPOPT::weight(unsigned int ch)
	{
	    vector<Node const *> const nodes = this->nodes();
	    unsigned long n = nodes.size();
	    unsigned long m = nchain();
	    
	    vector<double> w(n, 0.0);
	    for (unsigned int k = 0; k < n; ++k) {
		for (unsigned int j = 0; j < m; ++j) {
		    if (j != ch) {
			w[k] += exp(-nodes[k]->logDensity(j, PDF_FULL));
		    }
		}
		w[k] *= exp(-nodes[k]->logDensity(ch, PDF_FULL));
	    }
	    return w;
	}
	
    }
}
