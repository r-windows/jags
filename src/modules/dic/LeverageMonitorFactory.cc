#include <config.h>

#include "LeverageMonitorFactory.h"
#include "DensityEnums.h"
#include "LeverageStat.h"
#include "LooLeverageStat.h"
#include "LeverageTotalStat.h"

#include <model/BUGSModel.h>
#include <model/MeanMonitor.h>
#include <model/TraceMonitor.h>
#include <model/NodeArraySubset.h>
#include <graph/Node.h>

using std::string;
using std::vector;

namespace jags {
    namespace diag {

	template<class S>
	Monitor * newLeverageMonitor(vector<Node const *> const &nodes,
				    SummaryType summary_type,
				    vector<RNG*> &rngs,
				    unsigned int nrep)
	{
	    MonitorStat * stat = new  S(nodes, rngs, nrep);
	    Monitor *m = nullptr;
	    switch(summary_type) {
	    case TRACE:
		m =  new TraceMonitor(nodes, stat);
		break;
	    case MEAN:
		m = new MeanMonitor(nodes, stat);
		break;
	    case VAR:
	    case COV:
	    case STUNSET:
		delete stat;
		break; //-Wswitch
	    }
	    return m;
	}
	
	Monitor *LeverageMonitorFactory::getMonitor(string const &name, 
						   Range const &range,
						   BUGSModel *model,
						   string const &stat,
						   string const &summary,
						   string &msg)
	{
	    string nname = name;
	    string nstat = stat;
	    
	    if (name == "pD" && stat == "value") {
		/*
		  In JAGS 4.x.y. "pD" was defined as a virtual
		  node. We retain this for back-compatibility.
		*/
		if (model->symtab().getVariable("pD")) {
		    return nullptr; //Quit if we have a user-defined pD variable
		}
		nname = "_observed_";
		nstat = "leverage_total";
	    }
	    if (nstat != "leverage" && nstat != "loo_leverage" && nstat != "leverage_total") {
		return nullptr;
	    }
	    
	    vector<Node const *> nodes;
	    Range node_range = range;
	    if (nname == "_observed_") {
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

	    if (model->nchain() < 2) {
		msg = "At least two chains are required for leverage monitors";
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

	    vector<RNG*> rngs;	    
	    for (unsigned int i = 0; i < model->nchain(); ++i) {
		rngs.push_back(model->rng(i));
	    }

	    /* Create the correct subtype of monitor */

	    SummaryType summary_type = getSummaryType(summary);
	    if (summary_type != MEAN && summary_type != TRACE) return nullptr;
	    
	    Monitor *m = nullptr;
	    if (isWeighted(nstat)) {
		// loo_leverage
		m = newLeverageMonitor<LooLeverageStat>(nodes, summary_type, rngs, 10);
	    }
	    else if (isTotal(nstat)) {
		// leverage_total
		m = newLeverageMonitor<LeverageTotalStat>(nodes, summary_type, rngs, 10);
	    }
	    else {
		// Leverage
		m = newLeverageMonitor<LeverageStat>(nodes, summary_type, rngs, 10);
	    }
    
	    if (m) {
		/* Set name attributes */
		vector<string> elt_names;	  
		if (isTotal(nstat)) {
		    // Stats with only a single value
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

	string LeverageMonitorFactory::name() const
	{
	    return "diag::Leverage";
	}
	
    }
}
