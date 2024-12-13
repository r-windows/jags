#include <config.h>
#include <graph/Node.h>
#include <distribution/Distribution.h>
// Required for PDFtype enum
#include <util/nainf.h>

#include "DensityStat.h"

#include <cmath>
#include <stdexcept>

using std::vector;
using std::string;
using std::exp;
using std::logic_error;

#include <graph/Node.h>

namespace jags {
  
    namespace dic {

	DensityStat::DensityStat(vector<Node const *> const &nodes, 
				 DensityType density_type)
	    : MonitorStat(nodes.size()), _nodes(nodes), _density_type(density_type)
	{
	    // Sanity check that input arguments match to this function:
	
	    switch(density_type) {
	    case DENSITY:
	    case LOGDENSITY:
	    case DEVIANCE:
		break;
	    case DENSITY_TOTAL:
	    case LOGDENSITY_TOTAL:
	    case DEVIANCE_TOTAL:
		throw logic_error("Incompatible DensityType in DensityStat");
		break;
	    case DTUNSET:
		throw logic_error("Unimplemented DensityType in DensityStat");
		break;
	    }
	}

	vector<double> DensityStat::value(unsigned int ch) const
	{
	    vector<double> value(_nodes.size());
	    for (unsigned long i = 0; i < _nodes.size(); ++i) {
		double loglik = _nodes[i]->logDensity(ch, PDF_FULL);
		switch(_density_type) {
		case DENSITY:
		    value[i] = exp(loglik);
		    break;
		case DEVIANCE:
		    value[i] = -2.0 * loglik;
		    break;
		case LOGDENSITY:
		    value[i] = loglik;
		    break;
		case DENSITY_TOTAL:
		case LOGDENSITY_TOTAL:
		case DEVIANCE_TOTAL:
		case DTUNSET:
		    break; //-Wswitch
		}
	    }
	    return value;
	}
		
    }
}
