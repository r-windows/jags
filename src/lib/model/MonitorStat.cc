#include <config.h>

#include <model/MonitorStat.h>

using std::vector;

namespace jags {

    vector<double> MonitorStat::weight(unsigned int chain) const
    {
	return vector<double>();
    }

    bool MonitorStat::weighted() const
    {
	return false;
    }

}
