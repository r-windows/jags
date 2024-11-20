#include <config.h>
#include <graph/Node.h>
#include <distribution/Distribution.h>
// Required for PDFtype enum
#include <util/nainf.h>

#include "DensityMean.h"

#include <cmath>
#include <stdexcept>

using std::vector;
using std::string;
using std::exp;
using std::logic_error;

namespace jags {
  
namespace dic {

    DensityMean::DensityMean(vector<Node const *> const &nodes, 
			     DensityType density_type)
	: MeanMonitor(nodes, nodes.size()), _density_type(density_type)
    {
	// Sanity check that input arguments match to this function:
	
	switch(density_type) {
	case DENSITY:
	case LOGDENSITY:
	case DEVIANCE:
	    break;
	case DTUNSET:
	    throw logic_error("Unimplemented DensityType in DensityMean");
	}
    }

    vector<double> DensityMean::stat(unsigned int ch)
    {
	vector<Node const *> const &nodes = this->nodes();
	vector<double> value(nodes.size());
	for (unsigned long i = 0; i < nodes.size(); ++i) {
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
	
    vector<unsigned long> DensityMean::dim() const
    {
	return vector<unsigned long>(1, nodes().size());
    }
    
}}
