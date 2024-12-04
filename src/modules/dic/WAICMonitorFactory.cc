#include "WAICMonitorFactory.h"
#include "WAICMonitor.h"

#include <model/BUGSModel.h>
#include <graph/StochasticNode.h>

using std::string;
using std::vector;

namespace jags {
    namespace dic {
    
	Monitor *WAICMonitorFactory::getMonitor(string const &name,
						Range const &range,
						BUGSModel *model,
						string const &stat,
						string const &summary,
						string &msg)
	{
	    if (name != "WAIC")
		return nullptr;
	    if (stat != "value")
		return nullptr;
	    if (summary != "mean")
		return nullptr;

	    if (model->symtab().getVariable("WAIC")) {
		//Ignore user-defined WAIC variable
		return nullptr;
	    }
	    if (!isNULL(range))
		msg = string("Cannot monitor a subset of ") + name;
	    
	    vector<StochasticNode const *> observed_nodes;
	    vector<StochasticNode *> const &snodes = model->stochasticNodes();
	    for (auto p = snodes.begin(); p != snodes.end(); ++p) {
		if (isObserved(*p)) {
		    if (isParameter(*p)) {
			msg = "There are partly observed stochastic nodes";
			return nullptr;
		    }
		    else {
			observed_nodes.push_back(*p);
		    }
		}
	    }
	    if (observed_nodes.empty()) {
		msg = "There are no observed stochastic nodes";
		return nullptr;
	    }

	    Monitor *m = new WAICMonitor(observed_nodes);
	    vector<string> onames(observed_nodes.size());
	    for (unsigned int i = 0; i < observed_nodes.size(); ++i) {
		onames[i] = model->symtab().getName(observed_nodes[i]);
	    }
	    m->setElementNames(onames);

	    return m;
	}

	string WAICMonitorFactory::name() const
	{
	    return "dic::WAIC";
	}

    }
}
