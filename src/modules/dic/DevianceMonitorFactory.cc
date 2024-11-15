#include "DevianceMonitorFactory.h"
#include "DevianceMean.h"
#include "DevianceTrace.h"

#include <model/BUGSModel.h>
#include <graph/StochasticNode.h>

#include <set>

using std::set;
using std::string;
using std::vector;

namespace jags {
namespace dic {

    Monitor *DevianceMonitorFactory::getMonitor(string const &name, 
						Range const &range,
						BUGSModel *model,
						string const &stat,
						string const &summary,
						string &msg)
    {
	if (name != "deviance")
	    return nullptr;
	if (!isNULL(range)) {
	    msg = "cannot monitor a subset of deviance";
	    return nullptr;
	}
	if (stat != "value")
	    return nullptr;
	if (summary != "mean" && summary != "trace")
	    return nullptr;
	
	vector<StochasticNode *> const &snodes = model->stochasticNodes();
	vector<StochasticNode const *> observed_snodes;
	for (unsigned int i = 0; i < snodes.size(); ++i) {
	    if (isObserved(snodes[i])) {
		observed_snodes.push_back(snodes[i]);
	    }
	}
	if (observed_snodes.empty()) {
	    msg = "There are no observed stochastic nodes";
	    return nullptr;
	}

	Monitor *m = nullptr;
	if (summary == "mean") {
	    m = new DevianceMean(observed_snodes);
	    vector<string> onames(observed_snodes.size());
	    for (unsigned int i = 0; i < observed_snodes.size(); ++i) {
		onames[i] = model->symtab().getName(observed_snodes[i]);
	    }
	    m->setElementNames(onames);
	}
	else if (summary == "trace") {
	    m = new DevianceTrace(observed_snodes);
	    m->setElementNames(vector<string>(1,"deviance"));
	}
	return m;
    }

    string DevianceMonitorFactory::name() const
    {
	return "dic::Deviance";
    }

}}
