#include <config.h>

#include "CODA.h"
#include <model/BUGSModel.h>
#include <model/Monitor.h>
#include <model/NodeArray.h>
#include <model/MonitorFactory.h>
#include <graph/StochasticNode.h>
#include <graph/GraphMarks.h>
#include <graph/Node.h>
#include <rng/RNG.h>
#include <sampler/Sampler.h>
#include <util/dim.h>

#include <list>
#include <utility>
#include <stdexcept>
#include <fstream>
#include <cmath>

using std::vector;
using std::ofstream;
using std::list;
using std::pair;
using std::string;
using std::logic_error;
using std::runtime_error;
using std::map;

namespace jags {

/* 
   Nodes accessible to the user in a BUGSModel are identified
   by a variable name and range of indices 
*/
typedef pair<string, Range> NodeId;

BUGSModel::BUGSModel(unsigned int nchain)
    : Model(nchain), _symtab(this)
{
}

BUGSModel::~BUGSModel()
{
    for (auto i = monitors().begin(); i != monitors().end(); ++i) {
	delete i->monitor();
    }
}

SymTab &BUGSModel::symtab()
{
    return _symtab;
}

    static string monitorMsg(string const &name,
			     Range const &range,
			     string const &stat,
			     string const &summary)
    {
	return string("Monitor ") + name + 
	    printRange(range) +
	    " with stat " + stat + 
	    " and summary " + summary;
    }
    
void BUGSModel::coda(vector<NodeId> const &node_ids, string const &stem,
		     string &warn, string const &stat, string const &summary)
{
    warn.clear();
	
    list<MonitorControl> dump_monitors;
    for (auto i = node_ids.begin(); i != node_ids.end(); ++i) {
	string const &name = i->first;
	Range const &range = i->second;
	list<MonitorControl>::const_iterator p;
	for (p = monitors().begin(); p != monitors().end(); ++p) {
	    if (p->name() == name && p->range() == range
		&& (stat == "*" || p->stat() == stat)
		&& (summary == "*" || p->summary() == summary)) {
		// Wildcard * matches all stats and all summaries
		break;
	    }
	}
	if (p == monitors().end()) {
	    string msg = monitorMsg(name, range, stat, summary) + " not found.\n";
	    warn.append(msg);
	}
	else {
	    dump_monitors.push_back(*p);
	}
    }
    
    if (dump_monitors.empty()) {
	warn.append("There are no matching monitors\n");
	return;
    }
	
    unsigned int nwritten = 0;
    nwritten += CODA0(dump_monitors, stem, warn, stat, summary);    
    nwritten += CODA(dump_monitors, stem, nchain(), warn, stat, summary);
    nwritten += TABLE0(dump_monitors, stem, warn, stat, summary);    
    nwritten += TABLE(dump_monitors, stem, nchain(), warn, stat, summary);
	
    if (nwritten==0) {
	throw logic_error("Failed to write out monitors in CODA format.");
    }
	
}

void BUGSModel::coda(string const &stem, string &warn,
		     string const &stat, string const &summary)
{
    warn.clear();
    
    if (monitors().empty()) {
	warn.append("There are no monitors\n");
	return;
    }
    
    unsigned int nwritten = 0;
    nwritten += CODA0(monitors(), stem, warn, stat, summary);    
    nwritten += CODA(monitors(), stem, nchain(), warn, stat, summary);
    nwritten += TABLE0(monitors(), stem, warn, stat, summary);    
    nwritten += TABLE(monitors(), stem, nchain(), warn, stat, summary);

    if (nwritten == 0)  {
	throw logic_error("Failed to write out monitors in CODA format.");
    }
}


void BUGSModel::setParameters(map<string, SArray> const &param_table,
			      unsigned int chain)
{
    _symtab.writeValues(param_table, chain);


    //Strip off .RNG.seed (user-supplied random seed)
    if (param_table.find(".RNG.seed") != param_table.end()) {
	if (rng(chain) == nullptr) {
	    throw runtime_error(".RNG.seed supplied but RNG type not set");
	}
	SArray const &seed = param_table.find(".RNG.seed")->second;
	if (seed.length() != 1) {
	    throw runtime_error(".RNG.seed must be a single integer");
	}
	if (seed.value()[0] < 0) {
	    throw runtime_error(".RNG.seed must be non-negative");
	}
	unsigned int iseed = static_cast<unsigned int>(seed.value()[0]);
	rng(chain)->init(iseed);
    }

    //Strip off .RNG.state (saved state from previous run)
    if (param_table.find(".RNG.state") != param_table.end()) {
	if (rng(chain) == nullptr) {
	    throw runtime_error(".RNG.state supplied, but RNG type not set");
	}
	SArray const &state = param_table.find(".RNG.state")->second;
	vector<int> istate;
	//double const *value = state.value();
	vector<double> const &value = state.value();
	for (unsigned int i = 0; i < state.length(); ++i) {
	    istate.push_back(static_cast<int>(value[i]));
	}
	if (rng(chain)->setState(istate) == false) {
	    throw runtime_error("Invalid .RNG.state");
	}
    }
}


bool BUGSModel::setMonitor(string const &name, Range const &range,
			   unsigned int thin, string const &stat,
			   string const &summary,
			   string &msg)
{
    for (auto i = monitors().begin(); i != monitors().end(); ++i) {
	Monitor const *m = i->monitor();
	if (i->name() == name && i->range() == range && i->stat() == stat && i->summary() == summary) {
	    msg = "Monitor already exists and cannot be duplicated";
	    return false; 
	}
    }
    
    msg.clear();
    Monitor *monitor = nullptr;
    
    list<MonitorFactory*> const &faclist = monitorFactories();
    for(auto j = faclist.begin(); j != faclist.end(); ++j)
    {
	if ((*j)->isActive()) {
	    monitor = (*j)->getMonitor(name, range, this, stat, summary, msg);
	    if (monitor) {
		addMonitor(monitor, thin, name, range, stat, summary);
		return true;
	    }
	    else if (!msg.empty()) {
		return false;
	    }
	}
    }
    return false;
}

bool BUGSModel::deleteMonitor(string const &name, Range const &range,
			      string const &stat, string const &summary)
{
    for (auto i = monitors().begin(); i != monitors().end(); ++i)
    {
	Monitor *m = i->monitor();
	if (i->name() == name && i->range() == range && i->stat() == stat && i->summary() == summary) {
	    removeMonitor(m);
	    delete m;
	    return true;
	}
    }
    return false;
}

void BUGSModel::samplerNames(vector<vector<string> > &sampler_names) const
{
    sampler_names.clear();
    sampler_names.reserve(_samplers.size());

    for (unsigned int i = 0; i < _samplers.size(); ++i) {

	vector<string> names;	
	vector<StochasticNode *> const &nodes = _samplers[i]->nodes();
	names.reserve(nodes.size()+1);
	
	names.push_back(_samplers[i]->name());
	for (unsigned int j = 0; j < nodes.size(); ++j) {
	    names.push_back(_symtab.getName(nodes[j]));
	}
	sampler_names.push_back(names);
    }    
}

vector<Node const *> const &BUGSModel::observedStochasticNodes()
{
	/* Note: this could be implemented by conditionally including 
	_observed_stochastic_nodes.push_back(node) in a derived
	Model::addNode(StochasticNode) method - which would then 
	mean that observedStochasticNodes() could be const
	BUT this would add compilation time for all models and
	observedStochasticNodes are only needed by deviance monitors
	and to get the names of the observedStochasticNodes 
	Note sure what the best strategy is so will leave this for now */
	
	vector<StochasticNode*> const &snodes = stochasticNodes();

	_observed_stochastic_nodes.clear();
	for (unsigned int i = 0; i < snodes.size(); ++i) {
	    if (isObserved(snodes[i])) {
		// Implicit up-cast to Node from StochasticNode:
		_observed_stochastic_nodes.push_back(snodes[i]);
	    }
	}
	
	return _observed_stochastic_nodes;
}

void BUGSModel::dumpNodeNames(vector<string> &node_names,
			      string const &type, string &warn) const
{
    warn.clear();
    node_names.clear();
	
    if( type == "constant" ) {
	vector<Node *> const allnodes = nodes();
	for (unsigned int i = 0; i < allnodes.size(); ++i) {
	    if (allnodes[i]->isConstant()) {
		node_names.push_back(_symtab.getName(allnodes[i]));
	    }
	}
    }
    else if( type == "deterministic" ) {
	vector<Node *> const allnodes = nodes();
	for (unsigned int i = 0; i < allnodes.size(); ++i) {
	    if (allnodes[i]->isDeterministic()) {
		node_names.push_back(_symtab.getName(allnodes[i]));
	    }
	}
    }
    else if( type == "stochastic" ) {
	vector<StochasticNode *> const snodes = stochasticNodes();
	for (unsigned int i = 0; i < snodes.size(); ++i) {
	    // Implicit up-cast to Node from StochasticNode:
	    node_names.push_back(_symtab.getName(snodes[i]));
	}
    }
    else if( type == "fixed" ) {
	vector<Node *> const allnodes = nodes();
	for (unsigned int i = 0; i < allnodes.size(); ++i) {
	    if (allnodes[i]->isFixed()) {
		node_names.push_back(_symtab.getName(allnodes[i]));
	    }
	}
    }
    else if( type == "observed" ) {
	vector<StochasticNode *> const snodes = stochasticNodes();
	for (unsigned int i = 0; i < snodes.size(); ++i) {
	    if (isObserved(snodes[i])) {
		// Implicit up-cast to Node from StochasticNode:
		node_names.push_back(_symtab.getName(snodes[i]));
	    }
	}
    }
    else {
	warn.assign("retrieving node names for requested node type '");
	warn.append(type);
	warn.append("' is not implemented\n");
    }
}

} //namespace jags
