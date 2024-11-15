#include <config.h>
#include <graph/Node.h>
#include <distribution/Distribution.h>
// Required for PDFtype enum
#include <util/nainf.h>

#include "DensityPoolVariance.h"

#include <cmath>
#include <stdexcept>

using std::vector;
using std::string;
using std::exp;
using std::logic_error;

namespace jags {
namespace dic {

    DensityPoolVariance::DensityPoolVariance(vector<Node const *> const &nodes,
					     DensityType const density_type)
	: Monitor(nodes), 
	  _means(nodes.size(), 0.0), _mms(nodes.size(), 0.0), _variances(nodes.size(), 0.0),
	  _density_type(density_type), _nchain(nodes[0]->nchain()), _n(0)
    {
	switch(density_type) {
	case DENSITY:
	case LOGDENSITY:
	case DEVIANCE:
	    break;
	default:
	    throw logic_error("Unimplemented DensityType in DensityPoolVariance");
	}
    }

    void DensityPoolVariance::update()
    {
	vector<Node const *> const &nodes = this->nodes();
	for (unsigned int i = 0; i < nodes.size(); ++i) {
	    for (unsigned int ch = 0; ch < _nchain; ++ch) {
		double newval = nodes[i]->logDensity(ch, PDF_FULL);
		if (jags_isna(newval)) {
		    _means[i] = JAGS_NA;
		    _mms[i] = JAGS_NA;
		    _variances[i] = JAGS_NA;
		}
		else {
		    if( _density_type == DENSITY ) {
			newval = exp(newval);
		    }
		    else if ( _density_type == DEVIANCE ) {
			newval = -2.0 * newval;
		    }
					
		    // _n is incremented after the loop so the current sample is:
		    unsigned int effn = _n + ch + 1;
		    double delta = newval - _means[i];
					
		    _means[i] += delta / effn;
		    _mms[i] += delta * (newval - _means[i]);
		}
	    }
			
	    // Only necessary to update this once per iteration:
	    _variances[i] = _mms[i] / (double) (_n + _nchain - 1);			
	}
		
	// Here _n is the total number of iterations * chains:
	_n += _nchain;
	// Note: it is incremented after the loop as each chain counts as an iteration
		
    }
	
    vector<double> const &DensityPoolVariance::value(unsigned int ) const
    {
	return _variances;
    }

    vector<unsigned long> DensityPoolVariance::dim() const
    {
	return vector<unsigned long>(1, nodes().size());
    }

    bool DensityPoolVariance::poolChains() const
    {
	return true;
    }

    bool DensityPoolVariance::poolIterations() const
    {
	return true;
    }

}}
