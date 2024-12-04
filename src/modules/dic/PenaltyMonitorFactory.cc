#include "DensityEnums.h"
#include "PenaltyMonitorFactory.h"
#include "PenaltyPV.h"
//#include "PenaltyPOPTTotal.h"
//#include "PenaltyPOPTTotalRep.h"

#include "PDStat.h"
#include "POPTStat.h"
#include "PDTotalStat.h"

#include <model/BUGSModel.h>
#include <model/MeanMonitor.h>
#include <model/WeightedMeanMonitor.h>
#include <model/TraceMonitor.h>
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
	
	/*
	  Template constructor. T is a Monitor subtype and S is either
	  PDStat or POPTStat (Both stats take the same arguments in
	  the constructor). The common code avoids the need to create
	  a Monitor subclass for each combination of stat and summary.
	*/
	
	template<class T, class S>
	T * newPenaltyMonitor(vector<Node const *> const &nodes,
			      vector<RNG *> const &rngs,
			      unsigned int nrep)
	{
	    MonitorStat * stat = new S(nodes, rngs, nrep);
	    return new T(nodes, stat);
	}
	
	Monitor *PenaltyMonitorFactory::getMonitor(string const &name, 
						   Range const &range,
						   BUGSModel *model,
						   string const &stat,
						   string const &summary,
						   string &msg)
	{
	    PenaltyType penalty_type = PTUNSET;
	    if (name == "pD" && stat == "value") {
		/*
		  In JAGS 4.x.y. "pD" was defined as a virtual
		  node. We retain this for back-compatibility, but
		  this is equivalent to name = "_observed_" and stat =
		  "pD_total" in JAGS 5.0.0.
		*/
		if (model->symtab().getVariable("pD")) {
		    return nullptr; //Quit if we have a user-defined pD
		}
		penalty_type = PD_TOTAL;
	    }
	    else {
		penalty_type = getPenaltyType(stat);
	    }
	    if (penalty_type == PTUNSET) return nullptr;
	    
	    vector<Node const *> nodes;
	    Range node_range = range;
		    
	    if (name == "_observed_" || name == "pD") {
	   
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
		    return nullptr;
		}
		if (!isNULL(range) && !array->range().contains(range)) {
		    msg = string("Invalid subset ") + name + printRange(range);
		    return nullptr;
		}
		NodeArraySubset nodearray = NodeArraySubset(array, range);
		//FIXME: check for closure
		nodes = nodearray.nodes();
	    }


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
		    m = newPenaltyMonitor<MeanMonitor, PDStat>(nodes, rngs, 10);
		}
		else if (penalty_type == POPT) {
		    m = newPenaltyMonitor<WeightedMeanMonitor, POPTStat>(nodes, rngs, 10);
		}
		else if (penalty_type == PV) {
		    m = new PenaltyPV(nodes);
		}
	    }
	    else if (summary == "trace") {
		if (penalty_type == PD_TOTAL) {
		    m = newPenaltyMonitor<TraceMonitor, PDTotalStat>(nodes, rngs, 10);
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

	    vector<string> elt_names;	    
	    switch(penalty_type) {
	    case PD:
	    case POPT:
		// These stats have a single entry for each node
		for (auto p = nodes.begin(); p != nodes.end(); ++p) {
		    elt_names.push_back(model->symtab().getName(*p));
		}
		break;
	    case PV:
	    case PD_TOTAL:
	    case POPT_TOTAL:
		// These stats have only a single entry
		elt_names.push_back(name + printRange(range));
		break;
	    case PTUNSET:
		break; //-Wswitch
	    }
	    m->setElementNames(elt_names);

	    return m;
		
	}

	string PenaltyMonitorFactory::name() const
	{
	    return "dic::Penalty";
	}
	
    }
}
