#ifndef MONITOR_CONTROL_H_
#define MONITOR_CONTROL_H_

#include <sarray/Range.h>

#include <string>

namespace jags {

class Monitor;
class Range;
    
/**
 * @short Control a monitor 
 */
class MonitorControl {
    Monitor * _monitor;
    unsigned int _start;
    unsigned int _thin;
    unsigned int _niter;
    std::string const _name;
    Range const _range;
    std::string const _stat;
    std::string const _summary;
public:
    /** 
     * Constructor
     * @param monitor Monitor that will be under control
     * @param start   First iteration to be monitored
     * @param thin    Thinning interval for monitor
     * @param name    name of the object that is monitored
     * @param range   Range of the object that is monitored
     * @param stat    Name of the statistic reported by this Monitor
     * @param stat    Name of the summary reported by this Monitor
     */
    MonitorControl(Monitor *monitor, unsigned int start, unsigned int thin, std::string const &name, Range const &range, std::string const &stat, std::string const &summary);
    /**
     * Updates the monitor. If the iteration number coincides with
     * the thinning interval, then the update function of the Monitor
     * is called function is called.
     *
     * @param iteration The current iteration number.
     */
    void update(unsigned int iteration);
    /**
     * Reserves enough memory for a further niter iterations, taking
     * account of the thinning interval of the monitor.
     * @see Monitor#reserve
     */
    void reserve(unsigned int niter);
    /**
     * Returns the monitor under control.
     */
    Monitor *monitor() const;
    /**
     * First iteration monitored
     */
    unsigned int start() const;
    /**
     * Last iteration monitored
     */
    unsigned int end() const;
    /**
     * Thinning interval of monitor
     */
    unsigned int thin() const;
    /**
     * Number of iterations
     */
    unsigned int niter() const;
    /**
     * Returns the name used in the construction of the Monitor.
     */
    std::string const &name() const;
    /**
     * Returns the range used in the construction of the Monitor.
     */
    Range const &range() const;
    /**
     * Returns the name of the statistic recorded by the Monitor.
     */
    std::string const &stat() const;
    /**
     * Returns the name of the summary used by the Monitor.
     */
    std::string const &summary() const;

/**
     * Equality operator
     */
    //bool operator==(MonitorControl const &rhs) const; FIXME
};

} /* namespace jags */

#endif /* MONITOR_CONTROL_H_ */
