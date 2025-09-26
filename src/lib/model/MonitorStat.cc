#include <config.h>

#include <model/MonitorStat.h>
#include <util/dim.h>

using std::vector;
using std::string;

namespace jags {

    MonitorStat::MonitorStat(vector<unsigned long> const &dim,
			     vector<bool> const &missing)
	: _dim(dim), _length(product(dim)), _missing(missing)
    {
    }

    MonitorStat::MonitorStat(unsigned long length)
	: _dim(1, length), _length(length), _missing(length, false)
    {
    }

    MonitorStat::~MonitorStat()
    {
    }

    vector<unsigned long> const &MonitorStat::dim() const
    {
	return _dim;
    }

    unsigned long MonitorStat::length() const
    {
	return _length;
    }

    vector<bool> const &MonitorStat::missing() const
    {
	return _missing;
    }

    vector<double> MonitorStat::weight(unsigned int chain) const
    {
	return vector<double>();
    }

    WeightType MonitorStat::weighted() const
    {
	return UNWEIGHTED;
    }

    void MonitorStat::setNames(vector<string> const &names)
    {
	_names = names;
    }
    
    vector<string> const &MonitorStat::names() const
    {
	return _names;
    }
    
}
