#ifndef MONITOR_INFO_H_
#define MONITOR_INFO_H_

#include <string>
#include <sarray/Range.h>
#include <model/MonitorControl.h>

namespace jags {

/**
 * @short Information about a monitor 
 */
    class MonitorInfo : public MonitorControl {
    std::string _name;
    Range _range;
public:
    /** 
     * Constructor
     * @param monitor Monitor that will be under control
     * @param start   First iteration to be monitored
     * @param thin    Thinning interval for monitor
     * @param name Name of object to be monitored (typically a NodeArray)
     * @param range Range defining a subset of the object to be monitored
     */
	MonitorInfo(Monitor *monitor, unsigned int start, unsigned int thin,
		    std::string const &name, Range const &range);
    std::string const &name() const;
    Range const &range() const;
	//bool operator==(MonitorInfo const &rhs) const;
};

}

#endif /* MONITOR_INFO_H_ */
