#include <config.h>

#include "PDStat.h"

#include <graph/Node.h>

using std::vector;

namespace jags {

    namespace dic {

	PDStat::PDStat(vector<Node const *> const &nodes,
		       vector<RNG *> const &rngs, unsigned int nrep)
	    : MonitorStat(), _nodes(nodes), _rngs(rngs), _nrep(nrep)
	{
	}
    
	vector<unsigned long> PDStat::dim() const
	{
	    return vector<unsigned long>(1, _nodes.size());
	}

	unsigned long PDStat::length() const
	{
	    return _nodes.size();
	}
	
	vector<double> PDStat::value(unsigned int ch) const
	{
	    unsigned long m = _nodes[0]->nchain();
	    unsigned long n = _nodes.size();

	    vector<double> v(n);
	    for (unsigned int k = 0; k < n; ++k) {
		double pdsum = 0.0;
		for (unsigned int j = 0; j < m; ++j) {
		    if (j != ch) {
			pdsum += _nodes[k]->KL(ch, j, _rngs[ch], _nrep);
		    }
		}
		v[k] = pdsum/(n-1);
	    }
	    return v;
	}

    }
}
