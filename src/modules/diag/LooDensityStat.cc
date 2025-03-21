#include <config.h>

#include "LooDensityStat.h"

#include <graph/Node.h>

#include <cmath>
#include <stdexcept>

using std::vector;
using std::exp;
using std::logic_error;

namespace jags {
  
    namespace diag {
	
	LooDensityStat::LooDensityStat(vector<Node const *> const &nodes, 
				       DensityType density_type)
	    : MonitorStat(nodes.size()), _nodes(nodes), _density_type(density_type)
	{
	    // Sanity check that input arguments match to this function:
	    if (density_type == DTUNSET) {
		throw logic_error("Unimplemented DensityType in LooDensityStat");
	    }
	}

	vector<double> LooDensityStat::value(unsigned int ch) const
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
		case DTUNSET: 
		    break; //-Wswitch
		}
	    }
	    return value;
	}
	
	vector<double> LooDensityStat::weight(unsigned int ch) const
	{
	    vector<double> w(_nodes.size());
	    for (unsigned long i = 0; i < _nodes.size(); ++i) {
		w[i] = exp(- _nodes[i]->logDensity(ch, PDF_FULL));
	    }
	    return w;
	}
	
	WeightType LooDensityStat::weighted() const
	{
	    return VECTOR_WEIGHT;
	}
		
    }
}
