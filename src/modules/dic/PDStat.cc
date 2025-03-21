#include <config.h>

#include "PDStat.h"

#include <graph/Node.h>

using std::vector;

namespace jags {
    namespace dic {
	
	PDStat::PDStat(vector<Node const *> const &nodes,
				 vector<RNG *> const &rngs, unsigned int nrep)
	    : MonitorStat(1UL), _nodes(nodes), _rngs(rngs), _nrep(nrep)
	{
	}
	
	vector<double> PDStat::value(unsigned int ch) const
	{
	    unsigned long m = _nodes[0]->nchain();
	    unsigned long n = _nodes.size();
	    
	    double pdsum = 0;
	    for (unsigned int k = 0; k < n; ++k) {
		for (unsigned int j = 0; j < m; ++j) {
		    if (j != ch) {
			pdsum += _nodes[k]->KL(ch, j, _rngs[ch], _nrep);
		    }
		}
	    }

	    return vector<double>(1, pdsum/(m-1));
	}

    }
}
