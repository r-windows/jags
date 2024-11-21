#include "MeanMonitorFactory.h"
#include "ValueMeanMonitor.h"
//#include "PoolMeanMonitor.h"

#include <model/BUGSModel.h>
#include <graph/Graph.h>
#include <graph/Node.h>
#include <sarray/RangeIterator.h>

#include <stdexcept>

using std::set;
using std::string;
using std::vector;

namespace jags {
namespace base {

    Monitor *MeanMonitorFactory::getMonitor(string const &name,
					    Range const &range,
					    BUGSModel *model,
					    string const &stat,
					    string const &summary,
					    string &msg)
    {
	if (stat != "value")
	    return nullptr;
	
	//if (summary != "mean" && summary != "poolmean")
	if (summary != "mean")
	    return nullptr;

	NodeArray *array = model->symtab().getVariable(name);
	if (!array) {
	    msg = string("Variable ") + name + " not found";
	    return nullptr;
	}
	
	Monitor *m = nullptr;
	if (summary == "mean" ) {
	    m = new ValueMeanMonitor(NodeArraySubset(array, range));
	}
	/*
	else if (summary == "poolmean" ) {
	    m = new PoolMeanMonitor(NodeArraySubset(array, range));
	}
	else {
	    throw std::logic_error("Unimplemented MonitorType in MeanMonitorFactory");
	}
	*/
	
	//Set name attributes 
	//m->setName(name + printRange(range)); FIXME
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

    string MeanMonitorFactory::name() const
    {
	return "base::Mean";
    }
	
}}
