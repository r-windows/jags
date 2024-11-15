#include <config.h>
#include <graph/Node.h>
#include <distribution/Distribution.h>
// Required for PDFtype enum
#include <util/nainf.h>

#include "DensityPoolMean.h"

#include <cmath>
#include <stdexcept>

using std::vector;
using std::string;
using std::exp;
using std::logic_error;

namespace jags {
namespace dic {

    DensityPoolMean::DensityPoolMean(vector<Node const *> const &nodes,
				     DensityType density_type)
	: Monitor(nodes), _values(nodes.size(), 0.0),
	  _density_type(density_type), _nchain(nodes[0]->nchain()), _n(0) 
    {
	// Sanity check that input arguments match to this function:

	switch (density_type) {
	case DENSITY:
	case LOGDENSITY:
	case DEVIANCE:
	    break;
	default:
	    throw logic_error("Unimplemented DensityType in DensityPoolMean");
	}

	/*
	// Required for back-compatibility (only from ObsStochDensMonitorFactory):
	if ( monitor_name == "mean" ) {
	    if ( _density_type != DEVIANCE ) {
		throw logic_error("DensityPoolMean is reporting a non-DEVIANCE type with monitor_name mean");
	    }
	}
	*/
    }

    void DensityPoolMean::update()
    {
	_n++;
	vector<Node const*> const &nodes = this->nodes();
	for (unsigned int i = 0; i < nodes.size(); ++i) {
	    double newval = 0.0;
	    for (unsigned int ch = 0; ch < _nchain; ++ch) {
		newval += nodes[i]->logDensity(ch, PDF_FULL) / _nchain;
	    }
	    if (jags_isna(newval)) {
		_values[i] = JAGS_NA;
	    }
	    else {
		if( _density_type == DENSITY ) {
		    newval = exp(newval);
		}
		else if ( _density_type == DEVIANCE ) {
		    newval = -2.0 * newval;
		}
		_values[i] -= (_values[i] - newval)/_n;
	    }
	}
    }
    
    vector<double> const &DensityPoolMean::value(unsigned int) const
    {
	return _values;
    }

    vector<unsigned long> DensityPoolMean::dim() const
    {
	return vector<unsigned long>(1, nodes().size());
    }

    bool DensityPoolMean::poolChains() const
    {
	return true;
    }

    bool DensityPoolMean::poolIterations() const
    {
	return true;
    }

}}
