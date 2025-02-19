#include <config.h>
#include <graph/Node.h>

#include <algorithm>

#include <model/TraceMonitor.h>
#include <model/MonitorStat.h>
#include <util/nainf.h>

using std::vector;

namespace jags {

    TraceMonitor::TraceMonitor(vector<Node const *> const &nodes,
			       MonitorStat *stat)
	: Monitor(nodes), _stat(stat), _values(nchain())
    {
    }

    TraceMonitor::~TraceMonitor()
    {
    }
    
    void TraceMonitor::update(unsigned int chain)
    {
	vector<double> v = _stat->value(chain);
	vector<bool> const &missing = _stat->missing();
	for (unsigned int i = 0; i < v.size(); ++i) {
	    if (missing[i]) {
		v[i] = JAGS_NA;
	    }
	}
	_values[chain].insert(_values[chain].end(), v.begin(), v.end());
    }

    vector<double> TraceMonitor::value(unsigned int chain) const
    {
	return _values[chain];
    }

    unsigned long TraceMonitor::length() const
    {
	return _stat->length();
    }

    vector<unsigned long> TraceMonitor::dim() const
    {
	return _stat->dim();
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
