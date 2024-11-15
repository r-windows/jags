#include <config.h>
#include <graph/Node.h>
#include <distribution/Distribution.h>
// Required for PDFtype enum
#include <util/nainf.h>

#include "DensityTotal.h"

#include <cmath>
#include <stdexcept>

using std::vector;
using std::string;
using std::logic_error;
using std::exp;

namespace jags {
namespace dic {

    DensityTotal::DensityTotal(vector<Node const *> const &nodes, 
			       DensityType const density_type)
	: Monitor(nodes), _values(nodes[0]->nchain()),
	  _density_type(density_type), _nchain(nodes[0]->nchain())
    {
	// Sanity check that input arguments match to this function:
	switch(density_type) {
	case DENSITY:
	case LOGDENSITY:
	case DEVIANCE:
	    break;
	default:
	    throw logic_error("Unimplemented DensityType in DensityTotal");
	}

	/*
	// Required for back-compatibility (only from ObsStochDensMonitorFactory):
	if ( monitor_name == "trace" ) {
	    if ( _density_type != DEVIANCE ) {
		throw logic_error("DensityTotal is reporting a non-DEVIANCE type with monitor_name trace");
	    }
	}
	*/
    }

    void DensityTotal::update()
    {
	vector<Node const *> const &nodes = this->nodes();
	for (unsigned int ch = 0; ch < _nchain; ++ch) {
	    double total = 0.0;
	    for (auto p = nodes.begin(); p != nodes.end(); ++p) {
		total += (*p)->logDensity(ch, PDF_FULL);
	    }
	    if (jags_isna(total)) {
		// Don't try and convert NA to density or deviance
	    }
	    else if( _density_type == DENSITY ) {
		total = exp(total);
	    }
	    else if ( _density_type == DEVIANCE ) {
		total = -2.0 * total;
	    }
	    _values[ch].push_back(total);
	}
    }
	
    vector<double> const &DensityTotal::value(unsigned int chain) const
    {
	return _values[chain];
    }

    vector<unsigned long> DensityTotal::dim() const
    {
	return vector<unsigned long>(1, 1UL);
    }

    bool DensityTotal::poolChains() const
    {
	return false;
    }

    bool DensityTotal::poolIterations() const
    {
	return false;
    }

}}
