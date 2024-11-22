#include <config.h>
#include <graph/Node.h>
#include <distribution/Distribution.h>
// Required for PDFtype enum
#include <util/nainf.h>

#include "DensityTotalMean.h"

#include <cmath>
#include <stdexcept>

using std::vector;
using std::string;
using std::logic_error;
using std::exp;

namespace jags {
namespace dic {

    DensityTotalMean::DensityTotalMean(vector<Node const *> const &nodes, 
				       DensityType const density_type)
	: MeanMonitor(nodes, nodes.size()), _density_type(density_type)
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
    }

    vector<double> DensityTotalMean::stat(unsigned int ch)
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
	
    vector<unsigned long> DensityTotalMean::dim() const
    {
	return vector<unsigned long>(1, 1UL);
    }

}}
