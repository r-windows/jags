#include <config.h>

#include "WAICMonitor.h"
#include <graph/StochasticNode.h>
#include <module/ModuleError.h>
#include <rng/RNG.h>

#include <algorithm>

using std::vector;
using std::string;
using std::copy;
using std::fill;

namespace jags {

    static vector<Node const *> toNodeVec(vector<StochasticNode const *> const &s)
    {
	vector<Node const *> ans(s.size());
	copy (s.begin(), s.end(), ans.begin());
	return ans;
    }

    namespace dic {

	WAICMonitor::WAICMonitor(vector<StochasticNode const *> const &snodes)
	    : Monitor("mean", toNodeVec(snodes)), _snodes(snodes)
	      _mlik(_nchain, vector<double>(snodes.size(), 0.0)),
	      _vlik(_nchain, vector<double>(snodes.size(), 0.0))
	{
	}

	WAICMonitor::~WAICMonitor() 
	{
	}

	vector<unsigned long> WAICMonitor::dim() const
	{
	    return vector<unsigned long> (1, _snodes.size());
	}
 
	vector<double> const &WAICMonitor::value(vector<double> &v, unsigned int ch) const
	{
	    copy(_vlik[ch].begin(), _vlik[ch].end(), v.begin());
	}

	bool WAICMonitor::poolChains() const
	{
	    return false;
	}

	bool WAICMonitor::poolIterations() const
	{
	    return true;
	}

	void WAICMonitor::update(unsigned int ch)
	{
	    unsigned long n = niter();
	    for (unsigned int k = 0; k < _snodes.size(); ++k) {
		double delta = _snodes[k]->logDensity(ch, PDF_LIKELIHOOD) - _mlik[ch][k];
		if (n > 1) {
		    _vlik[ch][k] *= static_cast<double>(n - 2)/(n - 1);
		    _vlik[ch][k] += delta * delta / n;
		}
		_mlik[ch][k] += delta/n;
	    }
	}

    }
}
