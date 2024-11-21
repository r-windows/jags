#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include <model/MeanMonitor.h>

using std::vector;
using std::string;

namespace jags {

    MeanMonitor::MeanMonitor(vector<Node const *> const &nodes,
			     unsigned long statlength)
	: Monitor(nodes), _sums(nchain(), vector<double>(statlength, 0.0))
    {
    }
    
    void MeanMonitor::update(unsigned int ch)
    {
	vector<double> const &value = stat(ch);
	for (unsigned int i = 0; i < value.size(); ++i) {
	    _sums[ch][i] += value[i];
	}
    }

    void MeanMonitor::value(vector<double> &v, unsigned int chain) const
    {
	unsigned long n = niter();
	copy(_sums[chain].begin(), _sums[chain].end(), v.begin());
	for (unsigned int i = 0; i < v.size(); ++i) {
	    v[i] /= n;
	}
    }

    bool MeanMonitor::poolChains() const
    {
	return false;
    }
    
    bool MeanMonitor::poolIterations() const
    {
	return true;
    }
    
}
