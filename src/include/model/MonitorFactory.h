#ifndef MONITOR_FACTORY_H_
#define MONITOR_FACTORY_H_

#include <string>
#include <vector>

#include <util/Factory.h>

namespace jags {

class Monitor;
class Node;
class BUGSModel;
class Model;
class Range;

/**
 * @short Factory for Monitor objects
 */
class MonitorFactory : public Factory
{
public:
    ~MonitorFactory() override = default;
    /**
     * Creates a monitor of the given type by name and range. If a
     * monitor cannot be created, then a null pointer is returned.
     *
     * @param name Name of the monitored object. This is typically a 
     * node array.  
     *
     * @param range Range describing the subset, if any, to be monitored.
     * A NULL range is used to monitor the whole object
     *
     * @param model Pointer to a BUGSModel
     *
     * @param stat String indicating what statistic is to be monitored 
     * (e.g. "value", "logdensity", ...)
     *
     * @param summary String indicating how the monitored valeus are to be summarized
     * (e.g. "trace", "mean", "variance", ...)
     *
     * @param msg An error message may be written to this argument on
     * exit if the monitor cannot be created due to an error.  It is
     * not necessary to write an error message if the request for the
     * monitor lies outside the scope of the factory (e.g. the factory
     * cannot create monitors of the requested type). This is not considered
     * an error as all factories will be interrogated in turn until one
     * returns the requested monitor.
     */
    virtual Monitor *getMonitor(std::string const &name, Range const &range,
				BUGSModel *model,
				std::string const &stat,
				std::string const &summary,
				std::string &msg) = 0;
};

} /* namespace jags */

#endif /* MONITOR_FACTORY_H_ */
