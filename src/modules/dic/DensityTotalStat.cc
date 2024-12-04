#include <config.h>
#include <graph/Node.h>
#include <distribution/Distribution.h>
// Required for PDFtype enum
#include <util/nainf.h>

#include "DensityTotalStat.h"

#include <cmath>
#include <stdexcept>

using std::vector;
using std::string;
using std::logic_error;
using std::exp;

namespace jags {
    namespace dic {
	
	DensityTotalStat::DensityTotalStat(vector<Node const *> const &nodes, 
					   DensityType const density_type)
	    : MonitorStat(), _nodes(nodes), _density_type(density_type)
	{
	    // Sanity check that input arguments match to this function:
	    switch(density_type) {
	    case DENSITY_TOTAL:
	    case LOGDENSITY_TOTAL:
	    case DEVIANCE_TOTAL:
		break;
	    default:
		throw logic_error("Unimplemented DensityType in DensityTotal");
	    }
	}
	
	vector<double> DensityTotalStat::value(unsigned int ch) const
	{
	    double loglik = 0.0;
	    for (auto p = _nodes.begin(); p != _nodes.end(); ++p) {
		loglik += (*p)->logDensity(ch, PDF_FULL);
	    }
	
	    if (jags_isna(loglik)) {
		// Don't try and convert NA to density or deviance
	    }
	    else if ( _density_type == DENSITY_TOTAL ) {
		loglik = exp(loglik);
	    }
	    else if ( _density_type == DEVIANCE_TOTAL ) {
		loglik = -2.0 * loglik;
	    }
	    return vector<double>(1, loglik);
	}
	
	vector<unsigned long> DensityTotalStat::dim() const
	{
	    return vector<unsigned long>(1, 1UL);
	}

	unsigned long DensityTotalStat::length() const
	{
	    return 1UL;
	}
    }

}
