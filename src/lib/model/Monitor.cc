#include <config.h>
#include <model/Monitor.h>
#include <graph/StochasticNode.h>
#include <graph/Node.h>
#include <util/dim.h>

#include <stdexcept>

using std::string;
using std::vector;
using std::logic_error;
using std::copy;

namespace jags {

    Monitor::Monitor(vector<Node const *> const &nodes)
	: _nodes(nodes), _nchain(countChains(nodes)), _niter(0UL)
    {
    }

    Monitor::Monitor(Node const *node)
	: _nodes(vector<Node const*>(1,node)), _nchain(node->nchain()), _niter(0UL)
    {
    }

    Monitor::~Monitor()
    {}

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
	unsigned long statlength = product(dim());
	
	if (poolIterations()) {
	    return statlength;
	}
	else {
	    return statlength * niter();
	}
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

    vector<unsigned long> vdim = dim();
    unsigned long vlen = product(vdim);
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
