#include "DensityEnums.h"
#include "NodeDensityMonitorFactory.h"
#include "DensityStat.h"
#include "DensityTotalStat.h"

#include <model/BUGSModel.h>
#include <graph/Graph.h>
#include <graph/Node.h>
#include <model/NodeArraySubset.h>
#include <model/TraceMonitor.h>
#include <model/MeanMonitor.h>
#include <model/VarMonitor.h>
#include <sarray/RangeIterator.h>

using std::string;
using std::vector;

namespace jags {
namespace dic {

    template<class T, class S>
    T * newDensityMonitor(vector<Node const *> const &nodes,
			  DensityType density_type)
    {
	MonitorStat * stat = new  S(nodes, density_type);
	return new  T(nodes, stat);
    }

    Monitor *NodeDensityMonitorFactory::getMonitor(string const &name, 
						   Range const &range,
						   BUGSModel *model,
						   string const &stat,
						   string const &summary,
						   string &msg)
    {
	DensityType density_type = DTUNSET;
	if (name == "deviance" && stat == "value") {
	    /*
	      In JAGS 4.x.y. "deviance" was defined as a virtual
	      node. We retain this for back-compatibility, but
	      this is equivalent to name = "_observed_" and stat =
	      "deviance_total" in JAGS 5.0.0.
	    */
	    if (model->symtab().getVariable("deviance")) {
		return nullptr; //Quit if we have a user-defined deviance
	    }
	    density_type = DEVIANCE_TOTAL;
	}
	else {
	    density_type = getDensityType(stat);
	}
	if (density_type == DTUNSET) return nullptr;

	
	vector<Node const *> nodes;
	if (name == "_observed_" || name == "deviance") {
	    /* The name _observed_ is used when monitoring all
	       observed stochastic nodes.  The string _observed_ is
	       not a valid node name so is guaranteed not to clash
	       with a node.  If changing it then see also scanner.ll
	       and NodeDensityMonitorFactory.cc And also
	       parse.varname, waic.samples etc in rjags */

	    if (!isNULL(range)) {
		msg = string("Cannot take a subset of ") + name;
		return nullptr;
	    }

	    vector<Node const *> const &observed_snodes = model->observedStochasticNodes();
	    if (observed_snodes.empty()) {
		msg = "There are no observed stochastic nodes";
		return nullptr;
	    }
	    else {
		nodes = observed_snodes;
	    }
	}
	else {

	    NodeArray *array = model->symtab().getVariable(name);
	    if (!array) {
		// Not an error: name may refer to a virtual node
		return nullptr;
	    }
	    if (!isNULL(range) && !array->range().contains(range)) {
		msg = string("Invalid subset ") + name + printRange(range);
		return nullptr;
	    }
	    NodeArraySubset subset = NodeArraySubset(array, range);
	    //FIXME: check for closure
	    nodes = subset.nodes();
	}

	/* Create the correct subtype of monitor */

	vector<string> elt_names; //Element names
	Monitor *m = nullptr;
	switch(density_type) {
	case DENSITY_TOTAL:
	case LOGDENSITY_TOTAL:
	case DEVIANCE_TOTAL:
	    if (summary == "trace") {
		m = newDensityMonitor<TraceMonitor, DensityTotalStat>(nodes, density_type);
	    }
	    else if (summary == "mean") {
		m = newDensityMonitor<MeanMonitor, DensityTotalStat>(nodes, density_type);
	    }
	    else if (summary == "var") {
		m = newDensityMonitor<VarMonitor,  DensityTotalStat>(nodes, density_type);
	    }
	    // These stats are summarised between variables:
	    elt_names.push_back(name + printRange(range));
	    break;
	case DENSITY:
	case LOGDENSITY:
	case DEVIANCE:
	    if (summary == "trace") {
		m = newDensityMonitor<TraceMonitor, DensityStat>(nodes, density_type);
	    }
	    else if (summary == "mean") {
		m = newDensityMonitor<MeanMonitor, DensityStat>(nodes, density_type);
	    }
	    else if (summary == "var") {
		m = newDensityMonitor<VarMonitor, DensityStat>(nodes, density_type);
	    }
	    // These stats have a single entry for each node
	    for (auto p = nodes.begin(); p != nodes.end(); ++p) {
		elt_names.push_back(model->symtab().getName(*p));
	    }
	    break;
	case DTUNSET:
	    break; //-Wswitch
	}
	if (m) {
	    // Set name attributes
	    m->setElementNames(elt_names);
	}
	
	return m;
		
    }
    
    string NodeDensityMonitorFactory::name() const
    {
	return "dic::NodeDensity";
    }
	
}
}
