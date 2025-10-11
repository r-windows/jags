#include <config.h>
#include <model/Monitor.h>
#include <model/MonitorStat.h>
#include <graph/Node.h>
#include <util/dim.h>

#include <stdexcept>

using std::string;
using std::vector;
using std::logic_error;
using std::copy;

namespace jags {

    Monitor::Monitor(vector<Node const *> const &nodes, MonitorStat *stat)
	: _nodes(nodes), _nchain(countChains(nodes)), _niter(0UL), _stat(stat)
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

    unsigned long Monitor::length() const
    {
	return _stat->length();
    }

    vector<unsigned long> Monitor::dim() const
    {
	return _stat->dim();
    }

    vector<string> Monitor::elementNames() const
    {
	return _stat->names();
    }

    vector<vector<string>> Monitor::dimNames() const
    {
	unsigned long ndim = _stat->dim().size();
	if (ndim == 1UL) {
	    // Stat is a vector. Dimnames match element names.
	    return vector<vector<string>>(1, _stat->names());
	}
	else {
	    // Stat is an array. Leave dimnames empty.
	    return vector<vector<string>>(ndim, vector<string>());
	}
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

    vector<unsigned long> vdim = dim();
    if (flat) {
	vdim = vector<unsigned long>(1, nvalue);
    }
	
    vector<DimTag> tags(vdim.size(), DIM_VALUE);
    
    if (!poolIterations()) {
	vdim.push_back(niter);
	tags.push_back(DIM_ITER);
    }
    if (!poolChains()) {
	vdim.push_back(nchain);
	tags.push_back(DIM_CHAIN);
    }
	
    SArray ans(vdim);
    ans.setValue(v);    
    ans.setDimTags(tags);
    ans.setValueNames(elementNames());
    if (flat) {
	ans.setDimNames(elementNames(), 0);
    }
    else {
	vector<vector<string>> dimnames = dimNames();
	if (dimnames.size() > vdim.size()) {
	    throw logic_error("Size mismatch for Monitor dimnames");
	}
	for (unsigned long i = 0; i < dimnames.size(); ++i) {
	    ans.setDimNames(dimnames[i], i);
	}
    }
    return(ans);
}

} //namespace jags
