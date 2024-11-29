#include <config.h>

#include "PenaltyPDTotal.h"

using std::vector;

namespace jags {
    namespace dic {
	
	PDTotalTrace::PDTotalTrace(vector<Node const *> const &nodes,
				   vector<RNG *> const &rngs, unsigned int nrep)
	    : TraceMonitor(nodes), _rngs(rngs), _nrep(nrep)
	{
	}

	PDTotalTrace::~PDTotalTrace() 
	{
	}
	
	vector<unsigned long> PDTotalTrace::dim() const
	{
	    return vector<unsigned long>(1, 1UL);
	}
		
	vector<double> PDTotalTrace::stat(unsigned int ch)
	{
	    vector<Node const *> const &nodes = this->nodes();
	    unsigned long m = nchain();
	    unsigned long n = nodes.size();
	    
	    double pdsum = 0;
	    for (unsigned int k = 0; k < n; ++k) {
		for (unsigned int j = 0; j < m; ++j) {
		    if (j != ch) {
			pdsum += nodes[k]->KL(ch, j, _rngs[ch], _nrep);
		    }
		}
	    }

	    return vector<double>(1, pdsum/(m-1));
	}

    }
}
