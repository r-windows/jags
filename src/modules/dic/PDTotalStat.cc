#include <config.h>

#include "PDTotalStat.h"

#include <graph/Node.h>

using std::vector;

namespace jags {
    namespace dic {
	
	PDTotalStat::PDTotalStat(vector<Node const *> const &nodes,
				 vector<RNG *> const &rngs, unsigned int nrep)
	    : MonitorStat(), _nodes(nodes), _rngs(rngs), _nrep(nrep)
	{
	}
	
	vector<unsigned long> PDTotalStat::dim() const
	{
	    return vector<unsigned long>(1, 1UL);
	}
		
	vector<double> PDTotalStat::value(unsigned int ch) const
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

	unsigned long PDTotalStat::length() const
	{
	    return 1UL;
	}

    }
}
