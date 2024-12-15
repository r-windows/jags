#include <config.h>

#include <model/WeightMonitor.h>
#include <model/MonitorStat.h>

#include <stdexcept>

using std::vector;
using std::logic_error;

namespace jags {

    static vector<bool> weight_missing(MonitorStat const *stat)
    {
	/* Scalar weights are never missing, but vector weights have the same
	   missing pattern as the value vector */
	   
	vector<bool> missing;
	switch(stat->weighted()) {
	case UNWEIGHTED:
	    break;
	case SCALAR_WEIGHT:
	    missing.push_back(false);
	    break;
	case VECTOR_WEIGHT:
	    missing = stat->missing();
	    break;
	}
	return missing;
    }

    WeightMonitor::WeightMonitor(vector<Node const *> const &nodes, MonitorStat *stat)
	: Monitor(nodes), _stat(stat), _missing(weight_missing(stat))
    {
	/*
	  You should never need this so may as well make it a logic
	  error.  Discounting this option also allows us to simplify
	  the length and dim methods.
	*/
	if(stat->weighted() == UNWEIGHTED) {
	    throw logic_error("Cannot construct WeightMonitor for unweighted stat");
	}
    }

    WeightMonitor::~WeightMonitor()
    {
	delete _stat;
    }

    unsigned long WeightMonitor::length() const
    {
	if (_stat->weighted() == VECTOR_WEIGHT) {
	    return _stat->length();
	}
	return 1UL;
    }

    vector<unsigned long> WeightMonitor::dim() const
    {
	if (_stat->weighted() == VECTOR_WEIGHT) {
	    return _stat->dim();
	}
	return vector<unsigned long>(1, 1UL);
    }

    bool WeightMonitor::poolChains() const
    {
	return false;
    }

}
