#include <config.h>
#include <model/Monitor.h>
#include <model/MonitorStat.h>
#include <graph/StochasticNode.h>
#include <graph/Node.h>
#include <util/dim.h>

#include <stdexcept>

using std::string;
using std::vector;
using std::logic_error;
using std::copy;

namespace jags {

    Monitor::Monitor(vector<Node const *> const &nodes, MonitorStat *stat)
	: _nodes(nodes), _stat(stat), _nchain(countChains(nodes)), _niter(0UL)
    {
    }

    Monitor::~Monitor()
    {
	delete _stat;
    }

    void Monitor::update()
    {
	_niter++;
	if (this->poolChains()) {
	    update(0);
	}
	else {
	    for (unsigned int i = 0; i < _nchain; ++i) {
		update(i);
	    }
	}
    }
    
    vector<Node const*> const &Monitor::nodes() const
    {
	return _nodes;
    }

    unsigned long Monitor::niter() const
    {
	return _niter;
    }

    unsigned int Monitor::nchain() const
    {
	return _nchain;
    }

    unsigned long Monitor::size() const
    {
	if (poolIterations()) {
	    return _stat->length();
	}
	else {
	    return _stat->length() * niter();
	}
    }

    MonitorStat const *Monitor::stat() const
    {
	return _stat;
    }
    
    //FIXME: These should be in monitorinfo
    vector<string> const &Monitor::elementNames() const
    {
	return _elt_names;
    }
    
    void Monitor::setElementNames(vector<string> const &names)
    {
	_elt_names = names;
    }
    
SArray Monitor::dump(bool flat) const
{
    unsigned int nchain = poolChains() ? 1 : _nchain;
    unsigned long nvalue = this->size();
    
    vector<double> v(nvalue * nchain);
    vector<double>::iterator p = v.begin();
    for (unsigned int ch = 0; ch < nchain; ++ch) {
	vector<double> x(this->size());
	this->value(x, ch);
	p = copy(x.begin(), x.end(), p);
    }

    vector<unsigned long> vdim = _stat->dim();
    unsigned long vlen = _stat->length();
    if (nvalue % vlen != 0) {
	throw logic_error("Inconsistent dimensions in Monitor");
    }
    unsigned long niter = nvalue / vlen;
    if (poolIterations() && niter != 1) {
	throw logic_error("Invalid number of iterations in Monitor");
    }

    if (flat) {
	vdim = vector<unsigned long>(1, vlen);
    }
	
    vector<string> names(vdim.size(), "");

    if (!poolIterations()) {
	vdim.push_back(niter);
	names.push_back("iteration");
    }
    if (!poolChains()) {
	vdim.push_back(nchain);
	names.push_back("chain");
    }
	
    SArray ans(vdim);
    ans.setValue(v);    
    ans.setDimNames(names);
    if (flat) {
	ans.setSDimNames(_elt_names, 0);
    }
    return(ans);
}


} //namespace jags
