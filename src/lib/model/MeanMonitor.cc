#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include <model/MeanMonitor.h>
#include <model/MonitorStat.h>

using std::vector;

namespace jags {

    MeanMonitor::MeanMonitor(vector<Node const *> const &nodes, MonitorStat *stat)
	: Monitor(nodes, stat), _sums(nchain(), vector<double>(stat->length(), 0.0))
    {
    }

    MeanMonitor::~MeanMonitor()
    {
    }
    
    void MeanMonitor::update(unsigned int ch)
    {
	const vector<double> value = stat()->value(ch);
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
