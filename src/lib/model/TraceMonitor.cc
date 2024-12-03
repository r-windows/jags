#include <config.h>
#include <graph/Node.h>

#include <algorithm>

#include <model/TraceMonitor.h>
#include <model/MonitorStat.h>

using std::vector;

namespace jags {

    TraceMonitor::TraceMonitor(vector<Node const *> const &nodes,
			       MonitorStat *stat)
	: Monitor(nodes, stat), _values(nchain())
    {
    }

    TraceMonitor::~TraceMonitor()
    {
    }
    
    void TraceMonitor::update(unsigned int chain)
    {
	vector<double> v = stat()->value(chain);
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
