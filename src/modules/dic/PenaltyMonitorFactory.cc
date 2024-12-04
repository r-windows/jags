#include "DensityEnums.h"
#include "PenaltyMonitorFactory.h"
#include "PDMean.h"
#include "POPTMean.h"
#include "PenaltyPV.h"
#include "PDTotalTrace.h"
//#include "PenaltyPOPTTotal.h"
//#include "PenaltyPOPTTotalRep.h"

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

	Monitor *PenaltyMonitorFactory::getMonitor(string const &name, 
						   Range const &range,
						   BUGSModel *model,
						   string const &stat,
						   string const &summary,
						   string &msg)
	{
	    vector<Node const *> nodes;
	    Range node_range = range;
		    
	    if (name == "_observed_") {
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
		    //Not an error: name may be a virtual node
		    return nullptr;
		}
		if (isNULL(range)) {
		    //A null range corresponds to the whole array
		    node_range = array->range();
		}
		NodeArraySubset nodearray = NodeArraySubset(array, range);
		//FIXME: check for closure
		nodes = nodearray.nodes();
	    }

	    PenaltyType penalty_type = getPenaltyType(stat);
	    if (penalty_type == PTUNSET) return nullptr;
	    switch(penalty_type) {
	    case PD:
	    case POPT:
	    case PD_TOTAL:
	    case POPT_TOTAL:
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
	    }

	    vector<RNG*> rngs;	    
	    for (unsigned int i = 0; i < model->nchain(); ++i) {
		rngs.push_back(model->rng(i));
	    }

	    /* Create the correct subtype of monitor */

	    Monitor *m = nullptr;
	    if (summary == "mean") {
		if (penalty_type == PD) {
		    m = new PDMean(nodes, rngs, 10);
		}
		else if (penalty_type == POPT) {
		    m = new POPTMean(nodes, rngs, 10);
		}
		else if (penalty_type == PV) {
		    m = new PenaltyPV(nodes);
		}
	    }
	    else if (summary == "trace") {
		if (penalty_type == PD_TOTAL) {
		    m = new PDTotalTrace(nodes, rngs, 10);
		}
		/*
		else if (penalty_type == POPT_TOTAL) {
		    m = new PenaltyPOPTTotal(nodes, rngs, 10);
		}
		*/
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

	string PenaltyMonitorFactory::name() const
	{
	    return "dic::Penalty";
	}
	
    }
}
