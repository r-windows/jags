#include <config.h>
#include <graph/Node.h>
#include <util/nainf.h>

#include <algorithm>

#include <model/VarMonitor.h>
#include <model/MonitorStat.h>

using std::vector;

namespace jags {

    VarMonitor::VarMonitor(vector<Node const *> const &nodes,
			   MonitorStat const *stat)
	: Monitor(nodes), _stat(stat),
	  _sums(nchain(), vector<double>(stat->length(), 0.0)),
	  _sum_of_squares(nchain(), vector<double>(stat->length(), 0.0))
    {
    }

    VarMonitor::~VarMonitor()
    {
	delete _stat;
    }
    
    void VarMonitor::update(unsigned int chain)
    {
	vector<double> value = _stat->value(chain);
	vector<double> &S = _sums[chain];
	vector<double> &SS = _sum_of_squares[chain];		
	unsigned long n = niter();
	
	for (unsigned int i = 0; i < value.size(); ++i) {
	    if (n > 1) {
		double delta = value[i] - S[i]/(n-1);
		SS[i] += (n-1) * delta * delta / n;
	    }
	    S[i] += value[i];
	}
    }

    void VarMonitor::value(vector<double> &v, unsigned int chain) const
    {
	unsigned long n = niter();
	copy(_sum_of_squares[chain].begin(), _sum_of_squares[chain].end(), v.begin());
	for (unsigned int i = 0; i < v.size(); ++i) {
	    v[i] /= (n-1);
	}
    }

    bool VarMonitor::poolChains() const
    {
	return false;
    }

    bool VarMonitor::poolIterations() const
    {
	return true;
    }
    
    vector<unsigned long> VarMonitor::dim() const
    {
	return _stat->dim();
    }

}
