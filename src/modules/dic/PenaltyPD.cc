#include <config.h>

#include "PenaltyPD.h"

using std::vector;
using std::string;

namespace jags {

    namespace dic {

	PenaltyPD::PenaltyPD(vector<Node const *> const &nodes,
			     vector<RNG *> const &rngs, unsigned int nrep)
	    : MeanMonitor(nodes, nodes.size()), _rngs(rngs), _nrep(nrep)
	{
	}

	PenaltyPD::~PenaltyPD() 
	{
	}
    
	vector<unsigned long> PenaltyPD::dim() const
	{
	    return vector<unsigned long>(1, nodes().size());
	}
	
	vector<double> PenaltyPD::stat(unsigned int ch)
	{
	    unsigned long m = nchain();
	    vector<Node const *> const &nodes = this->nodes();
	    unsigned long n = nodes.size();

	    vector<double> v(n);
	    for (unsigned int k = 0; k < n; ++k) {
		double pdsum = 0.0;
		for (unsigned int j = 0; j < m; ++j) {
		    if (j != ch) {
			pdsum += nodes[k]->KL(ch, j, _rngs[ch], _nrep);
		    }
		}
		v[k] = pdsum/(n-1);
	    }
	    return v;
	}

    }
}
