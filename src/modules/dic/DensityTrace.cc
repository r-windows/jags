#include <config.h>
#include <graph/Node.h>
#include <distribution/Distribution.h>
// Required for PDFtype enum
#include <util/nainf.h>

#include "DensityTrace.h"

#include <cmath>
#include <stdexcept>

using std::vector;
using std::string;
using std::exp;
using std::logic_error;

namespace jags {
namespace dic {

    DensityTrace::DensityTrace(vector<Node const *> const &nodes,
			       DensityType const density_type)
	: Monitor(nodes), _values(nodes[0]->nchain()), _density_type(density_type)
    {
	// Sanity check that input arguments match to this function:
	switch(density_type) {
	case DENSITY:
	case LOGDENSITY:
	case DEVIANCE:
	    break;
	default:
	    throw logic_error("Unimplemented DensityType in DensityTrace");
	}
    }

    void DensityTrace::update()
    {
	vector<Node const *> const &nodes = this->nodes();
	for (unsigned int ch = 0; ch < _values.size(); ++ch) {
	    for (unsigned int i = 0; i < nodes.size(); ++i) {
		double newval = nodes[i]->logDensity(ch, PDF_FULL);
		if (jags_isna(newval)) {
		    // Don't try and convert NA to density or deviance
		}else if( _density_type == DENSITY ) {
		    newval = exp(newval);
		}
		else if ( _density_type == DEVIANCE ) {
		    newval = -2.0 * newval;
		}
		_values[ch].push_back(newval);
	    }
	}
    }
    
    vector<double> const &DensityTrace::value(unsigned int chain) const
    {
	return _values[chain];
    }

    vector<unsigned long> DensityTrace::dim() const
    {
	return vector<unsigned long>(1, nodes().size());
    }

    bool DensityTrace::poolChains() const
    {
	return false;
    }

    bool DensityTrace::poolIterations() const
    {
	return false;
    }

}}
