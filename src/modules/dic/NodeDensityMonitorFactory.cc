#include <config.h>

#include "NodeDensityMonitorFactory.h"
#include "DensityEnums.h"
#include "DensityStat.h"
#include "DensityTotalStat.h"
#include "LooDensityStat.h"

#include <model/BUGSModel.h>
#include <graph/Node.h>
#include <model/NodeArraySubset.h>
#include <model/TraceMonitor.h>
#include <model/MeanMonitor.h>
#include <model/VarMonitor.h>
#include <model/CovMonitor.h>

using std::vector;
using std::string;

namespace jags {
namespace dic {

    template<class S>
    Monitor * newDensityMonitor(vector<Node const *> const &nodes,
				DensityType density_type,
				SummaryType summary_type)
    {
	MonitorStat * stat = new  S(nodes, density_type);
	Monitor *m = nullptr;
	switch(summary_type) {
	case TRACE:
	    m =  new TraceMonitor(nodes, stat);
	    break;
	case MEAN:
	    m = new MeanMonitor(nodes, stat);
	    break;
	case VAR:
	    m = new VarMonitor(nodes, stat);
	    break;
	case COV:
	    m = new CovMonitor(nodes, stat);
	    break;
	case STUNSET:
	    delete stat;
	    break; //-Wswitch
	}
	return m;
    }

    Monitor *NodeDensityMonitorFactory::getMonitor(string const &name, 
						   Range const &range,
						   BUGSModel *model,
						   string const &stat,
						   string const &summary,
						   string &msg)
    {
	/* We may need to modify these arguments, so make a local copy.
	   We use the original name for error messages.
	 */
	string nname = name;
	string nstat = stat;
	
	if (name == "deviance" && stat == "value") {
	    /*
	      In JAGS 4.x.y. "deviance" was defined as a virtual
	      node. We retain this for back-compatibility.
	    */
	    if (model->symtab().getVariable("deviance")) {
		return nullptr; //Quit if we have a user-defined deviance variable
	    }
	    nname = "_observed_";
	    nstat = "deviance_total";
	}

	DensityType density_type = getDensityType(nstat);
	if (density_type == DTUNSET) return nullptr;

	
	vector<Node const *> nodes;
	if (nname == "_observed_") {
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

	    NodeArray *array = model->symtab().getVariable(nname);
	    if (!array) {
		// Not an error: name may refer to a virtual node so return silently
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
	SummaryType summary_type = getSummaryType(summary);
	
	Monitor *m = nullptr;
	if (isWeighted(nstat)) {
	    // loo_density, loo_logdensity, loo_deviance
	    m = newDensityMonitor<LooDensityStat>(nodes, density_type, summary_type);
	}
	else if (isTotal(nstat)) {
	    // density_total, logdensity_total, deviance_total
	    m = newDensityMonitor<DensityTotalStat>(nodes, density_type, summary_type);
	}
	else {
	    // density, logdensity, deviance
	    m = newDensityMonitor<DensityStat>(nodes, density_type, summary_type);
	}

	if (m) {
	    // Set name attributes
	    vector<string> elt_names;
	    if (isTotal(nstat)) {
		// Stats that are summarised between variables
		elt_names.push_back(name + printRange(range));
	    }
	    else {
		// Stats with a single entry for each node
		for (auto p = nodes.begin(); p != nodes.end(); ++p) {
		    elt_names.push_back(model->symtab().getName(*p));
		}
	    }
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
