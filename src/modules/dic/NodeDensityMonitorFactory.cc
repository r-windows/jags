#include "DensityEnums.h"
#include "NodeDensityMonitorFactory.h"
#include "DensityTrace.h"
#include "DensityMean.h"
#include "DensityVariance.h"
#include "DensityTotal.h"
//#include "DensityPoolMean.h"
//#include "DensityPoolVariance.h"
#include "PenaltyPD.h"
#include "PenaltyPOPT.h"
#include "PenaltyPV.h"
#include "PenaltyPDTotal.h"
#include "PenaltyPOPTTotal.h"
#include "PenaltyPOPTTotalRep.h"

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
	// Should never be true but just in case:
	if (name == "_observed_" ) {
	    return nullptr;
	}
		
	/* Retrieve the node array  */
		
	NodeArray *array = model->symtab().getVariable(name);
	if (!array) {
	    msg = string("Variable ") + name + " not found";
	    return nullptr;
	}
	NodeArraySubset nodearray = NodeArraySubset(array, range);
	//FIXME: check for closure
	
	/* Do some checks and create the RNG vector for pd and popt monitors */

	vector<RNG*> rngs;
	if (stat == "pD" || stat == "popt") {

	    //|| stat == "pD_total" || monitor_type == "popt_total" ||
	    //stat == POPTTOTALREP ) {

	    if (model->nchain() < 2) {
		msg = "at least two chains are required for a pD or popt monitor";
		return nullptr;
	    }
		    
	    /* 
	       We could limit pD/popt monitors to observed stochastic nodes only
	       But it does (maybe?) make sense as long as the parents of a node are unfixed
	       Otherwise it comes out as 0 anyway - which makes sense (no parents are estimated)
	       Note that pv can be calculated for any node with a density - which doesn't make sense
	       if the parents are fixed
	       TODO: create a node->areParentsFixed method to give an error for pv (and pD/popt??)
		       
	       // To limit pD / popt to observed stochastic nodes only (and pv if included above):
	       vector<Node const *> const &reqnodes = nodearray.allnodes();
	       for(unsigned int i = 0; i < reqnodes.size(); i++){
	       if ( !reqnodes[i]->isStochastic() ) {
	       msg = "non-stochastic nodes cannot be included in an array subset for a pD or popt monitor";
	       return 0;
	       }
	       if ( !reqnodes[i]->isFixed() ) {
	       msg = "unobserved nodes cannot be included in an array subset for a pD or popt monitor";
	       return 0;
	       }
	       }*/
			
	    for (unsigned int i = 0; i < model->nchain(); ++i) {
		rngs.push_back(model->rng(i));
	    }
	}

	/* Create the correct subtype of monitor */

	Monitor *m = nullptr;

	DensityType density_type = getDensityType(stat);
	PenaltyType penalty_type = getPenaltyType(stat);
	if (density_type != DTUNSET) {
	    if (density_type == DENSITY ||
		density_type == LOGDENSITY ||
		density_type == DEVIANCE)
	    {
		if (summary == "trace") {
		    m = new DensityTrace(nodearray.nodes(), density_type);
		}
		else if (summary == "mean") {
		    m = new DensityMean(nodearray.nodes(), density_type);
		}
		else if (summary == "variance") {
		    m = new DensityVariance(nodearray.nodes(), density_type);
		}
	    }
	    else {
		if (summary == "trace") {
		    m = new DensityTotal(nodearray.nodes(), density_type);
		}
	    }
	}
	else {
	    if (penalty_type == PD) {
		m = new PenaltyPD(nodearray.nodes(), rngs, 10);
	    }
	    else if (penalty_type == POPT) {
		m = new PenaltyPOPT(nodearray.nodes(), rngs, 10);
	    }
	    else if (penalty_type == PD_TOTAL) {
		m = new PenaltyPDTotal(nodearray.nodes(), rngs, 10);
	    }
	    else if (penalty_type == POPT_TOTAL) {
		m = new PenaltyPOPTTotal(nodearray.nodes(), rngs, 10);
	    }
	    else if (penalty_type == POPT_TOTAL_REP) {
		m = new PenaltyPOPTTotalRep(nodearray.nodes(), rngs, 10);
	    }
	    else if (penalty_type == PV) {
		m = new PenaltyPV(nodearray.nodes());
	    }
	    /*
	      else {
		throw std::logic_error("Unimplemented MonitorType in NodeDensityMonitorFactory");
	    }
	    */
	}
	
	if (!m) {
	    return nullptr;
	}
		
	/* Set name attributes */

	//m->setName(name + printRange(range));
	Range node_range = range;
	if (isNULL(range)) {
	    //A null range corresponds to the whole array
	    node_range = array->range();
	}

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
