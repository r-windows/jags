#include "DensityEnums.h"
#include "ObsStochDensMonitorFactory.h"
#include "DensityTrace.h"
#include "DensityMean.h"
#include "DensityVariance.h"
#include "DensityTotal.h"
#include "DensityPoolMean.h"
#include "DensityPoolVariance.h"
#include "PenaltyPD.h"
#include "PenaltyPOPT.h"
#include "PenaltyPV.h"
#include "PenaltyPDTotal.h"
#include "PenaltyPOPTTotal.h"
#include "PenaltyPOPTTotalRep.h"

#include <model/BUGSModel.h>
#include <graph/Graph.h>
#include <graph/Node.h>
#include <graph/StochasticNode.h>
#include <sarray/RangeIterator.h>

#include <set>
#include <stdexcept>

using std::set;
using std::string;
using std::vector;

namespace jags {
namespace dic {

    Monitor *ObsStochDensMonitorFactory::getMonitor(string const &name, 
						    Range const &range,
						    BUGSModel *model,
						    string const &stat,
						    string const &summary,
						    string &msg)
    {
		
	/* This factory is used when monitoring all observed
	   stochastic nodes.  The string _observations_ is not a valid
	   node name so is guaranteed not to clash with a node.  If
	   changing it then see also scanner.ll and
	   NodeDensityMonitorFactory.cc And also parse.varname,
	   waic.samples etc in rjags */

	if (name != "_observed_"
	    // For replacing DevianceMonitorFactory:
	    && !(name == "deviance" && stat == "value" && summary == "trace")
	    && !(name == "deviance" && stat == "value" && summary == "mean")
	    // For replacing PDMonitorFactory and PDTraceFactory:
	    && !(name == "pD" && stat == "value" && summary == "trace")
	    && !(name == "pD" && stat == "value" && summary == "mean")
	    //&& !(name == "popt" && type == "mean")
	    )
	    return nullptr;
	if (!isNULL(range)) {
	    msg = "cannot monitor a subset of all the observed stochastic nodes - use the specific node names and subsets instead";
	    return nullptr;
	}
	
	/* Work out the precise type of monitor */
		
	// enums declared in model/Monitor.h:
	//MonitorType monitor_type = MTUNSET; 
	//DensityType density_type = DTUNSET; 
		
	// Only needed for pd and popt-type monitors:
	vector<RNG*> rngs;

	/* These 5 duplicate DevianceMonitorFactory and PDMonitorFactory etc */

	/*
	bool matched = true;
	if (name == "deviance" && stat == "value" && type == "trace") {
	    // The equivalent of monitor('deviance', type='trace') in DevianceMonitorFactory:
	    
	    monitor_type = TOTAL;
	    density_type = DEVIANCE;
	}
	else if (name == "deviance" && type == "mean") {
	    // The equivalent of monitor('deviance', type='mean') in DevianceMonitorFactory:
	    monitor_type = POOLMEAN;
	    density_type = DEVIANCE;
	}
	else if (name == "pD" && type == "trace") {
	    monitor_type = PDTOTAL;
	}
	else if (name == "pD" && type == "mean") {
	    monitor_type = PD;
	}
	else if (name == "popt" && type == "mean") {
	    monitor_type = POPT;
	}
	else {
	    matched = getMonitorDensityTypes(type, monitor_type, density_type);
	}
	if (!matched) {
	    return nullptr;
	}
	*/

	/* Retrieve the node array  */
		
	vector<Node const *> const &observed_snodes = model->observedStochasticNodes();
		
	if (observed_snodes.empty()) {
	    msg = "There are no observed stochastic nodes";
	    return nullptr;
	}
		

	/* Do some checks and create the RNG vector for pd and popt monitors */

	/*
	if (monitor_type == PD || monitor_type == POPT
	    || monitor_type == PDTOTAL || monitor_type == POPTTOTAL ||
	    monitor_type == POPTTOTALREP ) {

	    if (model->nchain() < 2) {
		msg = "at least two chains are required for a pD or popt monitor";
		return nullptr;
	    }
			
	    for (unsigned int i = 0; i < model->nchain(); ++i) {
		rngs.push_back(model->rng(i));
	    }
	}
	*/

	/* Create the correct subtype of monitor */

	Monitor *m = nullptr;
		
	// There is only ever a single dimension of variables to worry about:
	vector<unsigned long> dim;
	dim.push_back(observed_snodes.size());

	/*
	switch (stat_type) {
	case DENSITY:
	case LOGDENSITY:
	case DEVIANCE:
	    switch(summary_type) {
	    case TRACE:
		m = new DensityTrace(observed_snodes, dim, density_type, type);
		break;
	    case MEAN:
		m = new DensityMean(observed_snodes, dim, density_type, type);
		break;
		
		if (stat_type ==  || stat == "logdensity" || stat == "
	if (monitor_type == TRACE) {

	}
	else if (monitor_type == MEAN) {

	}
	else if (monitor_type == VARIANCE) {
	    m = new DensityVariance(observed_snodes, dim, density_type, type);
	}
	else if (monitor_type == TOTAL) {
	    m = new DensityTotal(observed_snodes, dim, density_type, type);
	}
	else if (monitor_type == POOLMEAN) {
	    m = new DensityPoolMean(observed_snodes, dim, density_type, type);
	}
	else if (monitor_type == POOLVARIANCE) {
	    m = new DensityPoolVariance(observed_snodes, dim, density_type, type);
	}
	else if (monitor_type == PD) {
	    m = new PenaltyPD(observed_snodes, dim, type, rngs, 10);
	}
	else if (monitor_type == POPT) {
	    m = new PenaltyPOPT(observed_snodes, dim, type, rngs, 10);
	}
	else if (monitor_type == PDTOTAL) {
	    m = new PenaltyPDTotal(observed_snodes, type, rngs, 10);
	}
	else if (monitor_type == POPTTOTAL) {
	    m = new PenaltyPOPTTotal(observed_snodes, type, rngs, 10);
	}
	else if (monitor_type == POPTTOTALREP) {
	    m = new PenaltyPOPTTotalRep(observed_snodes, type, rngs, 10);
	}
	else if (monitor_type == PV) {
	    m = new PenaltyPV(observed_snodes, type);
	}
	else {
	    throw std::logic_error("Unimplemented MonitorType in ObsStochDensMonitorFactory");
	}
	*/
	
	if (!m ) {
	    return nullptr;
	}
		
	/* Set name attributes */
		
	// Will either be _observations_, or for backwards compatibility: deviance, pD, or popt
        //m->setName(name);

	// These types are summarised between variables:
		    /*
		    if (monitor_type == TOTAL || monitor_type == PDTOTAL
	    || monitor_type == POPTTOTAL || monitor_type == PV) {
	    m->setElementNames(vector<string>(1, type));
	}
	else {
	    vector<string> onames;
	    string msg;
	    model->dumpNodeNames(onames, "observed", msg);
	    if ( onames.size() != observed_snodes.size() ) {
		throw std::logic_error("The number of observed stochastic nodes does not match the length of the names");
	    }
	    if (!msg.empty()) {
		throw std::logic_error(msg.c_str());
	    }
	    m->setElementNames(onames);
	}
		    */
		
	return m;
		
    }

    string ObsStochDensMonitorFactory::name() const
    {
		return "dic::ObsStochDensity";
    }
	
}
}
