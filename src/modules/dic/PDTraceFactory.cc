#include "PDTraceFactory.h"
#include "PDTrace.h"

#include <model/BUGSModel.h>
#include <graph/StochasticNode.h>
#include <distribution/Distribution.h>

#include <set>

using std::set;
using std::string;
using std::vector;

namespace jags {
namespace dic {

    Monitor *PDTraceFactory::getMonitor(string const &name,
					Range const &range,
					BUGSModel *model,
					string const &stat,
					string const &summary,
					string &msg)
    {
	if (name != "pD") 
	    return nullptr;

	if (!isNULL(range)) {
	    msg = "cannot monitor a subset of pD";
	    return nullptr;
	}

	if (stat != "value")
	    return nullptr;
	
	if (summary != "trace")
	    return nullptr;

	if (model->nchain() < 2) {
	    msg = "at least two chains are required for a pD trace monitor";
	    return nullptr;
	}

	vector<StochasticNode const *> observed_nodes;
	vector<StochasticNode *> const &snodes = model->stochasticNodes();
	for (unsigned int i = 0; i < snodes.size(); ++i) {
	    if (snodes[i]->isFixed()) {
		observed_nodes.push_back(snodes[i]);
	    }
	    if (!isSupportFixed(snodes[i])) {
		msg = "pD is infinite because at least one observed node does not have fixed support";
		return nullptr;
	    }
	}
	if (observed_nodes.empty()) {
	    msg = "there are no observed nodes";
	    return nullptr;
	}

	unsigned int nchain = model->nchain();
	vector<RNG*> rngs;
	for (unsigned int i = 0; i < nchain; ++i) {
	    rngs.push_back(model->rng(i));
	}

	Monitor *m  = new PDTrace(observed_nodes, rngs, 10);
	m->setElementNames(vector<string>(1,"pD"));
	return m;
    }

    string PDTraceFactory::name() const
    {
	return "dic::PDTrace";
    }

}}
