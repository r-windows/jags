#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include "PoolVarianceMonitor.h"

using std::vector;
using std::string;

namespace jags {
namespace base {

    PoolVarianceMonitor::PoolVarianceMonitor(NodeArraySubset const &subset)
	: Monitor(subset.nodes()), _subset(subset),
	  _sums(subset.length(), 0.0),
	  _sum_of_squares(subset.length(), 0.0)
    {
    }
    
    void PoolVarianceMonitor::update(unsigned int)
    {
	unsigned long n = niter();
	unsigned long m = nchain();
	unsigned long p = _subset.length();
	
	vector<double> ysum(p, 0.0); //sum across chains
	for (unsigned int ch = 0; ch < nchain(); ++ch) {
	    vector<double> value = _subset.value(ch);
	    for (unsigned int i = 0; i < p; ++i) {
		ysum[i] += value[i];
	    }
	}

	vector<double> yss(p, 0.0); //sum of squares across chains
	for (unsigned int ch = 0; ch < nchain(); ++ch) {
	    vector<double> value = _subset.value(ch);
	    for (unsigned int i = 0; i < p; ++i) {
		double delta = value[i] - ysum[i]/m;
		yss[i] += delta * delta;
	    }
	}

	for (unsigned int i = 0; i < p; ++i) {
	    _sum_of_squares[i] += yss[i];
	    if (n > 1) {
		double delta = ysum[i] - _sums[i]/(n - 1);
		_sum_of_squares[i] += (n -1) * delta * delta / n;
	    }
	    _sums[i] += ysum[i];
	}
    }


    void PoolVarianceMonitor::value(vector<double> &v, unsigned int) const
    {
	unsigned long df = niter() * nchain() - 1;
	copy(_sum_of_squares.begin(), _sum_of_squares.end(), v.begin());
	for (unsigned long i = 0; i < v.size(); ++i) {
	    v[i] /= df;
	}
    }
    
    vector<unsigned long> PoolVarianceMonitor::dim() const
    {
	return _subset.dim();
    }

     bool PoolVarianceMonitor::poolChains() const
    {
	return true;
    }

    bool PoolVarianceMonitor::poolIterations() const
    {
	return true;
    }
	
}}
