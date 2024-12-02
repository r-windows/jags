#include "DensityEnums.h"
#include "NodeDensityMonitorFactory.h"
#include "DensityTrace.h"
#include "DensityMean.h"
#include "DensityVariance.h"
#include "DensityTotal.h"
#include "DensityTotalMean.h"
#include "DensityTotalVar.h"

#include <model/BUGSModel.h>
#include <graph/Graph.h>
#include <graph/Node.h>
#include <model/NodeArraySubset.h>
#include <sarray/RangeIterator.h>

#include <set>
#include <stdexcept>

using std::set;
using std::string;
using std::vector;

namespace jags {
namespace dic {

    Monitor *NodeDensityMonitorFactory::getMonitor(string const &name, 
						   Range const &range,
						   BUGSModel *model,
						   string const &stat,
						   string const &summary,
						   string &msg)
    {
	DensityType density_type = DTUNSET;
	if (name == "deviance") {
	    /* For compatibility with JAGS 4.0 we allow the user to
	     * monitor 'deviance' as if it were a virtual node */
	    if (stat != "value") return nullptr;
	    if (!isNULL(range)) return nullptr;
	    density_type = DEVIANCE_TOTAL;
	}
	else {
	    density_type = getDensityType(stat);
	}
	if (density_type == DTUNSET) return nullptr;

	Range node_range = range;
	vector<Node const *> nodes;
	if (name == "_observed_" || name == "deviance") {
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
	    if (isNULL(range)) {
		//A null range corresponds to the whole array
		node_range = array->range();
	    }
	    else if (!array->range().contains(range)) {
		msg = string("Invalid subset ") + name + printRange(range);
		return nullptr;
	    }

	    NodeArraySubset nodearray = NodeArraySubset(array, range);
	    //FIXME: check for closure
	    nodes = nodearray.nodes();
	}

	/* Create the correct subtype of monitor */

	vector<string> elt_names; //Element names
	Monitor *m = nullptr;
	switch(density_type) {
	case DENSITY_TOTAL:
	case LOGDENSITY_TOTAL:
	case DEVIANCE_TOTAL:
	    if (summary == "trace") {
		m = new DensityTotalTrace(nodes, density_type);
	    }
	    else if (summary == "mean") {
		m = new DensityTotalMean(nodes, density_type);
	    }
	    else if (summary == "var") {
		m = new DensityTotalVar(nodes, density_type);
	    }
	    // These stats are summarised between variables:
	    elt_names.push_back(name + printRange(range));
	    break;
	case DENSITY:
	case LOGDENSITY:
	case DEVIANCE:
	    if (summary == "trace") {
		m = new DensityTrace(nodes, density_type);
	    }
	    else if (summary == "mean") {
		m = new DensityMean(nodes, density_type);
	    }
	    else if (summary == "var") {
		m = new DensityVariance(nodes, density_type);
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
