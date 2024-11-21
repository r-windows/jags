#include <config.h>
#include <graph/Node.h>

#include <algorithm>

#include <model/TraceMonitor.h>

using std::vector;
using std::string;

namespace jags {

    TraceMonitor::TraceMonitor(vector<Node const *> const &nodes)
	: Monitor(nodes),  _values(nchain())
    {
    }
    
    void TraceMonitor::update(unsigned int chain)
    {
	vector<double> const &v = stat(chain);
	_values[chain].insert(_values[chain].end(), v.begin(), v.end());
    }

    void TraceMonitor::value(vector<double> &v, unsigned int chain) const
    {
	copy(_values[chain].begin(), _values[chain].end(), v.begin());
    }
    
    bool TraceMonitor::poolChains() const
    {
	return false;
    }

    bool TraceMonitor::poolIterations() const
    {
	return false;
    }

}
