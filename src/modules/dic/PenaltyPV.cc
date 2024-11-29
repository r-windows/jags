#include <config.h>

#include "PenaltyPV.h"

#include <util/nainf.h>
#include <distribution/Distribution.h>

using std::vector;

namespace jags {
    namespace dic {

	PenaltyPV::PenaltyPV(vector<Node const *> const &nodes)
	    : VarMonitor(nodes, 1UL)
	{
	}

	vector<double> PenaltyPV::stat(unsigned int ch)
	{
	    vector<Node const *> const &nodes = this->nodes();
	    double loglik = 0.0;
	    for (unsigned int i = 0; i < nodes.size(); ++i) {
		loglik += nodes[i]->logDensity(ch, PDF_FULL);
	    }
	    return vector<double>(1, loglik);
	
	}

	vector<unsigned long> PenaltyPV::dim() const
	{
	    return vector<unsigned long>(1, 1UL);
	}

    }
}
