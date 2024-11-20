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
	: TraceMonitor(nodes), _density_type(density_type)
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

    vector<double> DensityTrace::stat(unsigned int ch)
    {
	vector<Node const *> const &nodes = this->nodes();
	vector<double> values(nodes.size());
	for (unsigned int i = 0; i < nodes.size(); ++i) {
	    double loglik = nodes[i]->logDensity(ch, PDF_FULL);
	    switch(_density_type) {
	    case DENSITY:
		values[i] = exp(loglik);
		break;
	    case DEVIANCE:
		values[i] = -2.0 * loglik;
		break;
	    case LOGDENSITY: case DTUNSET:
		values[i] = loglik;
		break;
	    }
	}
	return values;
    }
    
    vector<unsigned long> DensityTrace::dim() const
    {
	return vector<unsigned long>(1, nodes().size());
    }

}}
