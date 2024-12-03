#include "DevianceMonitorFactory.h"
#include "DensityMean.h"
#include "DensityTrace.h"
#include "DensityVariance.h"

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
	    if (name != "deviance") {
		if(model->symtab().getVariable("deviance")) {
		    // Ignore if a variable named "deviance" is defined in the model
		    return nullptr;
		}
	    }
	    else if (name != "_deviance_") {
		return nullptr;
	    }

	    if (!isNULL(range)) {
		msg = string("cannot monitor a subset of ") + name;
		return nullptr;
	    }

	    if (stat != "value")
		return nullptr;

	    if (summary != "mean" && summary != "trace" && summary != "var")
		return nullptr;
	
	    vector<StochasticNode *> const &snodes = model->stochasticNodes();
	    vector<Node const *> observed_snodes;
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
		m = new DensityMean(observed_snodes, DEVIANCE_TOTAL);
	    }
	    if (summary == "var") {
		m = new DensityVariance(observed_snodes, DEVIANCE_TOTAL);
	    }
	    else if (summary == "trace") {
		m = new DensityTrace(observed_snodes, DEVIANCE_TOTAL);
	    }
	    if (m) {
		m->setElementNames(vector<string>(1,"deviance"));
	    }
	    return m;
	}

	string DevianceMonitorFactory::name() const
	{
	    return "dic::Deviance";
	}

    }
}
