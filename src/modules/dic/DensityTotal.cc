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
	: TraceMonitor(nodes), _density_type(density_type)
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

    vector<double> DensityTotal::stat(unsigned int ch)
    {
	double loglik = 0.0;
	for (auto p = nodes().begin(); p != nodes().end(); ++p) {
	    loglik += (*p)->logDensity(ch, PDF_FULL);
	}

	if (jags_isna(loglik)) {
	    // Don't try and convert NA to density or deviance
	}
	else if ( _density_type == DENSITY ) {
	    loglik = exp(loglik);
	}
	else if ( _density_type == DEVIANCE ) {
	    loglik = -2.0 * loglik;
	}
	return vector<double>(1, loglik);
    }
	
    vector<unsigned long> DensityTotal::dim() const
    {
	return vector<unsigned long>(1, 1UL);
    }

}}
