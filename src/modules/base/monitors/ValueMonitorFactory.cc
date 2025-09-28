#include "ValueMonitorFactory.h"
#include "ValueStat.h"

#include <model/BUGSModel.h>
#include <model/TraceMonitor.h>
#include <model/MeanMonitor.h>
#include <model/VarMonitor.h>
#include <model/CovMonitor.h>
#include <sarray/RangeIterator.h>

using std::string;
using std::vector;
using std::pair;

namespace jags {
    namespace base {

	/*
	  Template constructor for monitors that use ValueStat as the
	  stat object. The common code avoids the need to create a
	  Monitor subclass for each summary type.
	*/
	template <class T>
	pair<T*,ValueStat*>
	newValueMonitor(NodeArray *array, Range const &range)
	{
	    NodeArraySubset subset(array, range);
	    ValueStat *stat = new ValueStat(subset);
	    T *m = new T(subset.nodes(), stat);
	    return pair<T*,ValueStat*>(m, stat);
	}
	
	Monitor *ValueMonitorFactory::getMonitor(string const &name,
						 Range const &range,
						 BUGSModel *model,
						 string const &stat,
						 string const &summary,
						 string &msg)
	{
	    if (stat != "value")
		return nullptr;

	    NodeArray *array = model->symtab().getVariable(name);
	    if (!array) {
		/* This is not necessarily an error. The name may be a
		   virtual node (e.g. "deviance") or a collection
		   (e.g. "_observed_") that is handled by another
		   Monitor factory. Hence no message.
		*/
		return nullptr;
	    }
	    if (!isNULL(range) && !array->range().contains(range)) {
		msg = string("Invalid subset ") + name + printRange(range);
		return nullptr;
	    }
	    
	    pair<Monitor*, MonitorStat*> m(nullptr, nullptr);
	    if (summary == "trace") {
		m = newValueMonitor<TraceMonitor>(array, range);
	    }
	    else if (summary == "mean") {
		m = newValueMonitor<MeanMonitor>(array, range);
	    }
	    else if (summary == "var" || summary == "variance") {
		m = newValueMonitor<VarMonitor>(array, range);
	    }
	    else if (summary == "cov") {
		m = newValueMonitor<CovMonitor>(array, range);
	    }

	    if (!m.first) {
		return nullptr;
	    }

	    //Set name attributes 
	    Range node_range = range;
	    if (isNULL(range)) {
		//A null range corresponds to the whole array
		node_range = array->range();
	    }
	    vector<string> stat_names;
	    if (node_range.length() > 1) {
		for (RangeIterator i(node_range); !i.atEnd(); i.nextLeft()) {
		    stat_names.push_back(name + printIndex(i));
		}
	    }
	    else {
		stat_names.push_back(name + printRange(range));
	    }
	    m.second->setNames(stat_names);
	
	    return m.first;
	}

	string ValueMonitorFactory::name() const
	{
	    return "base::Value";
	}

    }
}
