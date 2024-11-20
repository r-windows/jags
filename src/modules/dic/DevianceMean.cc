#include "DevianceMean.h"

#include <graph/StochasticNode.h>

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

    DevianceMean::DevianceMean(vector<StochasticNode const *> const &s)
	: Monitor(toNodeVec(s)), _snodes(s), _mdev(s.size(), 0.0)
    {
    }

    vector<unsigned long> DevianceMean::dim() const
    {
	return vector<unsigned long>(1, _mdev.size());
    }

    bool DevianceMean::poolChains() const
    {
	return true;
    }

    bool DevianceMean::poolIterations() const
    {
	return true;
    }

    void DevianceMean::value(vector<double> &v, unsigned int chain) const
    {
	copy(_mdev.begin(), _mdev.end(), v.begin());
    }

    void DevianceMean::update(unsigned int)
    {
	unsigned long n = niter();
	unsigned long m = nchain();
	
	for (unsigned long i = 0; i < _snodes.size(); ++i) {
	    double loglik = 0;
	    for (unsigned int ch = 0; ch < m; ++ch) {
		loglik += _snodes[i]->logDensity(ch, PDF_FULL);
	    }
	    _mdev[i] += (-2*loglik/m - _mdev[i])/n;
	}
    }

}}
