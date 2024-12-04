#include <config.h>

#include "WAICMonitor.h"
#include "DensityStat.h"

#include <graph/StochasticNode.h>

#include <algorithm>

using std::vector;
using std::copy;

namespace jags {

    static vector<Node const *> toNodeVec(vector<StochasticNode const *> const &s)
    {
	vector<Node const *> ans(s.size());
	copy (s.begin(), s.end(), ans.begin());
	return ans;
    }

    namespace dic {

	WAICMonitor::WAICMonitor(vector<StochasticNode const *> const &snodes)
	    : VarMonitor(toNodeVec(snodes), new DensityStat(toNodeVec(snodes), LOGDENSITY))
	{
	}

	WAICMonitor::~WAICMonitor() 
	{
	}

    }
}
