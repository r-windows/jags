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

	Monitor *m = nullptr;
	if (density_type == DENSITY ||
	    density_type == LOGDENSITY ||
	    density_type == DEVIANCE)
	{
	    if (summary == "trace") {
		m = new DensityTrace(nodes, density_type);
	    }
	    else if (summary == "mean") {
		m = new DensityMean(nodes, density_type);
	    }
	    else if (summary == "variance") {
		m = new DensityVariance(nodes, density_type);
	    }
	}
	else if (density_type == DENSITY_TOTAL ||
		 density_type == LOGDENSITY_TOTAL ||
		 density_type == DEVIANCE_TOTAL)
	{
	    if (summary == "trace") {
		m = new DensityTotalTrace(nodes, density_type);
	    }
	    else if (summary == "mean") {
		m = new DensityTotalMean(nodes, density_type);
	    }
	    else if (summary == "variance") {
		m = new DensityTotalVar(nodes, density_type);
	    }
	}
	if (!m) {
	    return nullptr;
	}
		
	/* Set name attributes */

	/** FIXME: Does not work for multivariate nodes
	// These types are summarised between variables:
	if (monitor_type == TOTAL || monitor_type == PDTOTAL
	    || monitor_type == POPTTOTAL || monitor_type == PV) {
	    m->setElementNames(vector<string>(1, type));
	}
	else {
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
	}
	*/
	return m;
		
    }

    string NodeDensityMonitorFactory::name() const
    {
	return "dic::NodeDensity";
    }
	
}
}
