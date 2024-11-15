#include <config.h>
#include <model/MonitorInfo.h>
#include <model/MonitorControl.h>

using std::string;

namespace jags {

    MonitorInfo::MonitorInfo(Monitor *monitor, unsigned int start, unsigned int thin,
			     string const &name, Range const &range)
	: MonitorControl(monitor, start, thin, name, range, "", ""), _name(name), _range(range)
    {
    }

    string const &MonitorInfo::name() const
    {
	return _name;
    }
    
    Range const &MonitorInfo::range() const
    {
	return _range;
    }


} //namespace jags
