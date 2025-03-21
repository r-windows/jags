#include <config.h>

#include "DensityTotalStat.h"

#include <graph/Node.h>

#include <cmath>
#include <stdexcept>

using std::vector;
using std::logic_error;
using std::exp;

namespace jags {
    namespace diag {
	
	DensityTotalStat::DensityTotalStat(vector<Node const *> const &nodes, 
					   DensityType const density_type)
	    : MonitorStat(1UL), _nodes(nodes), _density_type(density_type)
	{
	    if (density_type == DTUNSET) {
		throw logic_error("Unimplemented DensityType in DensityTotal");
	    }
	}
	
	vector<double> DensityTotalStat::value(unsigned int ch) const
	{
	    double loglik = 0.0;
	    for (auto p = _nodes.begin(); p != _nodes.end(); ++p) {
		loglik += (*p)->logDensity(ch, PDF_FULL);
	    }

	    switch(_density_type) {
	    case DENSITY:
		loglik = exp(loglik);
		break;
	    case DEVIANCE:
		loglik = -2.0 * loglik;
		break;
	    case LOGDENSITY: case DTUNSET:
		break;
	    }
	    return vector<double>(1, loglik);
	}
	
    }

}
