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
	: Monitor(nodes),
	  _values(nodes[0]->nchain(), vector<double>(nodes.size(), 0.0)),
	  _density_type(density_type), _n(0)
    {
	// Sanity check that input arguments match to this function:

	switch(density_type) {
	case DENSITY:
	case LOGDENSITY:
	case DEVIANCE:
	    break;
	default:
	    throw logic_error("Unimplemented DensityType in DensityMean");
	}
    }

    void DensityMean::update()
    {
	_n++;
	vector<Node const *> const &nodes = this->nodes();
	for (unsigned int ch = 0; ch < _values.size(); ++ch) {
	    vector<double> &rmean  = _values[ch];
	    for (unsigned long i = 0; i < nodes.size(); ++i) {
		double newval = nodes[i]->logDensity(ch, PDF_FULL);
		if (jags_isna(newval)) {
		    rmean[i] = JAGS_NA;
		}
		else if( _density_type == DENSITY ) {
		    newval = exp(newval);
		}
		else if (_density_type == DEVIANCE ) {
		    newval = -2.0 * newval;
		}
		    rmean[i] += (newval - rmean[i])/_n;
	    }
	}
    }
	
    vector<double> const &DensityMean::value(unsigned int chain) const
    {
	return _values[chain];
    }

    vector<unsigned long> DensityMean::dim() const
    {
	return vector<unsigned long>(1, nodes().size());
    }

    bool DensityMean::poolChains() const
    {
	return false;
    }

    bool DensityMean::poolIterations() const
    {
	return true;
    }

}}
