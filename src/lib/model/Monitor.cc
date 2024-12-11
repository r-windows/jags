#include <config.h>
#include <model/Monitor.h>
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

    Monitor::~Monitor()
    {
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
    unsigned int niter = poolIterations() ? 1 : _niter;
    unsigned long nvalue = this->length();
    
    vector<double> v(nvalue * niter * nchain);
    vector<double>::iterator p = v.begin();
    for (unsigned int ch = 0; ch < nchain; ++ch) {
	const vector<double> x = this->value(ch);
	p = copy(x.begin(), x.end(), p);
    }

    if (poolIterations() && niter != 1) {
	throw logic_error("Invalid number of iterations in Monitor");
    }

    vector<unsigned long> vdim = dim();
    if (flat) {
	vdim = vector<unsigned long>(1, nvalue);
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
