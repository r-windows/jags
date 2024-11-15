#include "PDMonitorFactory.h"
#include "PDMonitor.h"
#include "PoptMonitor.h"

#include <model/BUGSModel.h>
#include <graph/StochasticNode.h>
#include <distribution/Distribution.h>

#include <set>

using std::set;
using std::string;
using std::vector;

namespace jags {
namespace dic {

    Monitor *PDMonitorFactory::getMonitor(string const &name,
					  Range const &range,
					  BUGSModel *model,
					  string const &stat,
					  string const &summary,
					  string &msg)
    {
	if (name != "pD" && name != "popt")
	    return nullptr;	
	
	if (!isNULL(range)) {
	    msg = string("Cannot monitor a subset of ") + name;
	}

	if (stat != "value")
	    return nullptr;
	
	if (summary != "mean")
	    return nullptr;
	
	if (model->nchain() < 2) {
	    msg = string("At least two parallel chains needed to monitor ")
		+ name;
	    return nullptr;
	}

	vector<StochasticNode const *> observed_nodes;
	vector<StochasticNode *> const &snodes = model->stochasticNodes();
	for (unsigned int i = 0; i < snodes.size(); ++i) {
	    if (snodes[i]->isFixed()) {
		if (isSupportFixed(snodes[i])) {
		    observed_nodes.push_back(snodes[i]);
		}
		else {
		    msg = "Support of observed nodes is not fixed";
		    return nullptr;
		}
	    }
	}
	if (observed_nodes.empty()) {
	    msg = "There are no observed stochastic nodes";
	    return nullptr;
	}

	unsigned int nchain = model->nchain();
	vector<RNG*> rngs;
	for (unsigned int i = 0; i < nchain; ++i) {
	    rngs.push_back(model->rng(i));
	}

	Monitor *m = nullptr;
	if (name =="pD") {
	    m = new PDMonitor(observed_nodes, rngs, 10);
	}
	else if (name == "popt") {
	    m = new PoptMonitor(observed_nodes, rngs, 10);
	}
	if (m) {
	    vector<string> onames(observed_nodes.size());
	    for (unsigned int i = 0; i < observed_nodes.size(); ++i) {
		onames[i] = model->symtab().getName(observed_nodes[i]);
	    }
	    m->setElementNames(onames);
	}
	return m;
    }

    string PDMonitorFactory::name() const
    {
	return "dic::PD";
    }

}}
