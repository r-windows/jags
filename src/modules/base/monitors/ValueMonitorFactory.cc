#include "ValueMonitorFactory.h"
#include "ValueTraceMonitor.h"
#include "ValueMeanMonitor.h"
#include "ValueVarMonitor.h"

#include <model/BUGSModel.h>
#include <graph/Graph.h>
#include <graph/Node.h>
#include <sarray/RangeIterator.h>

using std::set;
using std::string;
using std::vector;

namespace jags {
    namespace base {

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
		msg = string("Variable ") + name + " not found";
		return nullptr;
	    }
	    
	    Monitor *m = nullptr;
	    if (summary == "trace") {
		m = new ValueTraceMonitor(NodeArraySubset(array, range));
	    }
	    else if (summary == "mean") {
		m = new ValueMeanMonitor(NodeArraySubset(array, range));
	    }
	    else if (summary == "var") {
		m = new ValueVarMonitor(NodeArraySubset(array, range));
	    }

	    if (!m) {
		return nullptr;
	    }

	    //Set name attributes 
	    Range node_range = range;
	    if (isNULL(range)) {
		//A null range corresponds to the whole array
		node_range = array->range();
	    }
	    vector<string> elt_names;
	    if (node_range.length() > 1) {
		for (RangeIterator i(node_range); !i.atEnd(); i.nextLeft()) {
		    elt_names.push_back(name + printIndex(i));
		}
	    }
	    else {
		elt_names.push_back(name + printRange(range));
	    }
	    m->setElementNames(elt_names);
	
	    return m;
	}

	string ValueMonitorFactory::name() const
	{
	    return "base::Value";
	}

    }
}
