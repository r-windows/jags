#include <config.h>
#include <graph/Node.h>
#include <distribution/Distribution.h>
// Required for PDFtype enum
#include <util/nainf.h>

#include "DensityVariance.h"

#include <cmath>
#include <stdexcept>

using std::vector;
using std::string;
using std::logic_error;
using std::exp;

namespace jags {
namespace dic {

    DensityVariance::DensityVariance(vector<Node const *> const &nodes, DensityType const density_type)
	: Monitor(nodes),
	  _means(nodes[0]->nchain(), vector<double>(nodes.size(), 0.0)),
	  _mms(nodes[0]->nchain(), vector<double>(nodes.size(), 0.0)),
	  _variances(nodes[0]->nchain(), vector<double>(nodes.size(), 0.0)),
	  _density_type(density_type), _n(0)
    {
	
	// Sanity check that input arguments match to this function:
	switch(density_type) {
	case DENSITY:
	case LOGDENSITY:
	case DEVIANCE:
	    break;
	default:
	    throw logic_error("Unimplemented DensityType in DensityVariance");
	}
    }

    void DensityVariance::update()
    {
	_n++;
	vector<Node const *> const &nodes = this->nodes();
	for (unsigned int ch = 0; ch < _means.size(); ++ch) {
	    vector<double> &rmean  = _means[ch];
	    vector<double> &rmm  = _mms[ch];
	    vector<double> &rvar  = _variances[ch];		
	    for (unsigned int i = 0; i < nodes.size(); ++i) {
		double newval = nodes[i]->logDensity(ch, PDF_FULL);
		if (jags_isna(newval)) {
		    rmean[i] = JAGS_NA;
		    rmm[i] = JAGS_NA;
		    rvar[i] = JAGS_NA;
		}
		else {
		    if(_density_type == DENSITY ) {
			newval = exp(newval);
		    }
		    else if ( _density_type == DEVIANCE ) {
			newval = -2.0 * newval;
		    }
		    
		    double delta = newval - rmean[i];
		    rmean[i] += delta / _n;
		    rmm[i] += delta * (newval - rmean[i]);
		    rvar[i] = rmm[i] / (double) (_n - 1);
		}
	    }
	}
    }
    
    vector<double> const &DensityVariance::value(unsigned int chain) const
    {
	return _variances[chain];
    }

    vector<unsigned long> DensityVariance::dim() const
    {
	return vector<unsigned long>(1, nodes().size());
    }

    bool DensityVariance::poolChains() const
    {
	return false;
    }

    bool DensityVariance::poolIterations() const
    {
	return true;
    }

}}
