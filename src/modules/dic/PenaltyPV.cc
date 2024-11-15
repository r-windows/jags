#include <config.h>

#include "PenaltyPV.h"

#include <util/nainf.h>
#include <distribution/Distribution.h>
// Required for PDFtype enum

using std::vector;
using std::string;

namespace jags {
namespace dic {

    PenaltyPV::PenaltyPV(vector<Node const *> const &nodes)
	: Monitor(nodes), _mean(0.0), _mm(0.0),	_pv(1,0.0),
	  _nchain(nodes[0]->nchain()), _n(0)
    {
    }

    void PenaltyPV::update()
    {
	vector<Node const *> const &nodes = this->nodes();
	for (unsigned int ch = 0; ch < _nchain; ++ch) {
	    double newval = 0.0;
	    for (unsigned int i = 0; i < nodes.size(); ++i) {
		newval -= 2.0 * nodes[i]->logDensity(ch, PDF_FULL);
	    }
	    if (jags_isna(newval)) {
		_mean = JAGS_NA;
		_mm = JAGS_NA;
		_pv[0] = JAGS_NA;
	    }
	    else {
		// _n is incremented after the loop so the current sample is:
		unsigned int effn = _n + ch + 1;
		double delta = newval - _mean;
				
		_mean += delta / effn;
		_mm += delta * (newval - _mean);
	    }			
	}

	// Only necessary to update this once per iteration:
	_pv[0] = _mm / ( 2.0 * static_cast<double>(_n + _nchain - 1) );
			 
	// Here _n is the total number of iterations * chains:
	_n += _nchain;
	// Note: it is incremented after the loop as each chain counts as an iteration
		
    }
	
    vector<double> const &PenaltyPV::value(unsigned int ) const
    {
	return _pv;
    }

    vector<unsigned long> PenaltyPV::dim() const
    {
	return vector<unsigned long>(1, 1UL);
    }

    bool PenaltyPV::poolChains() const
    {
	return true;
    }

    bool PenaltyPV::poolIterations() const
    {
	return true;
    }

}}
