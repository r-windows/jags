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
	: VarMonitor(nodes, nodes.size()), _density_type(density_type)
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

    vector<double> DensityVariance::stat(unsigned int ch)
    {
	vector<Node const *> const &nodes = this->nodes();
	vector<double> value(nodes.size());
	for (unsigned int i = 0; i < nodes.size(); ++i) {
	    double loglik = nodes[i]->logDensity(ch, PDF_FULL);
	    switch(_density_type) {
	    case DENSITY:
		value[i] = exp(loglik);
		break;
	    case DEVIANCE:
		value[i] = -2.0 * loglik;
		break;
	    case LOGDENSITY: case DTUNSET:
		value[i] = loglik;
		break;
	    }
	}
	return value;
    }
    
    vector<unsigned long> DensityVariance::dim() const
    {
	return vector<unsigned long>(1, nodes().size());
    }

}}
