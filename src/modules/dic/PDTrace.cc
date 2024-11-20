#include <config.h>

#include "PDTrace.h"
#include <graph/StochasticNode.h>
#include <module/ModuleError.h>

#include <algorithm>

using std::vector;
using std::string;
using std::copy;

namespace jags {

static vector<Node const *> toNodeVec(vector<StochasticNode const *> const &s)
{
    vector<Node const *> ans(s.size());
    copy (s.begin(), s.end(), ans.begin());
    return ans;
}

namespace dic {

    PDTrace::PDTrace(vector<StochasticNode const *> const &snodes,
		     vector<RNG *> const &rngs, unsigned int nrep)
	: Monitor(toNodeVec(snodes)),
	  _snodes(snodes), _rngs(rngs), _nrep(nrep),
	  _values()
    {
	if (countChains(nodes()) < 2) {
	    throwLogicError("PDTrace needs at least 2 chains");
	}
    }

    PDTrace::~PDTrace() 
    {
    }

    vector<unsigned long> PDTrace::dim() const
    {
	return vector<unsigned long> (1,1UL);
    }
 
    void PDTrace::value(vector<double> &v, unsigned int ) const
    {
	copy(_values.begin(), _values.end(), v.begin());
    }

    bool PDTrace::poolChains() const
    {
	return true;
    }

    bool PDTrace::poolIterations() const
    {
	return false;
    }

    void PDTrace::update(unsigned int)
    {
	unsigned int m = nchain();
	
	double pd = 0;
	for (unsigned int k = 0; k < _snodes.size(); ++k) {
	    for (unsigned int i = 0; i < m; ++i) {
		for (unsigned int j = 0; j < i; ++j) {
		    pd += _snodes[k]->KL(i, j, _rngs[i], _nrep) + _snodes[k]->KL(j, i, _rngs[j], _nrep);
		}
	    }
	}
	pd /= m * (m - 1);
	_values.push_back(pd);
    }

}}
