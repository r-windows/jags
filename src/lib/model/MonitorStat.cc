#include <config.h>

#include <model/MonitorStat.h>

using std::vector;

namespace jags {

    vector<double> MonitorStat::weight(unsigned int chain) const
    {
	return vector<double>();
    }

    WeightType MonitorStat::weighted() const
    {
	return UNWEIGHTED;
    }

}
