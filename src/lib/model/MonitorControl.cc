#include <config.h>
#include <model/MonitorControl.h>
#include <model/Monitor.h>

#include <stdexcept>

using std::invalid_argument;
using std::string;

namespace jags {

MonitorControl::MonitorControl (Monitor *monitor, unsigned int start, 
				unsigned int thin, string const &name,
				Range const &range, string const &stat,
				string const &summary)
    : _monitor(monitor), _start(start), _thin(thin), _niter(0), _name(name), _range(range), _stat(stat), _summary(summary)
{
   if (thin == 0) {
	throw invalid_argument("Illegal thinning interval");
    }
}

unsigned int MonitorControl::start() const
{
    return _start;
}

unsigned int MonitorControl::end() const
{
    return _start + _thin * _niter;
}

unsigned int MonitorControl::niter() const
{
    return _niter;
}

unsigned int MonitorControl::thin() const
{
    return _thin;
}

Monitor *MonitorControl::monitor() const
{
    return _monitor;
}

    string const &MonitorControl::name() const
    {
	return _name;
    }

    Range const &MonitorControl::range() const
    {
	return _range;
    }

    string const &MonitorControl::stat() const
    {
	return _stat;
    }

    string const &MonitorControl::summary() const
    {
	return _summary;
    }

void MonitorControl::update(unsigned int iteration)
{
    if (iteration < _start || (iteration - _start) % _thin != 0) {
	return;
    }
    else {
	_monitor->update();
	_niter++;
    }
}


} //namespace jags
