#include <config.h>
#include <Console.h>
#include <compiler/Compiler.h>
#include <compiler/parser_extra.h>
#include <compiler/ParseTree.h>
#include <model/BUGSModel.h>
#include <model/Monitor.h>
#include <graph/NodeError.h>
#include <sampler/SamplerFactory.h>
#include <model/MonitorFactory.h>
#include <rng/RNGFactory.h>
#include <graph/Node.h>
#include <sarray/Range.h>
#include <sarray/SArray.h>
#include <rng/RNG.h>
#include <util/dim.h>
#include <module/Module.h>
#include <graph/StochasticNode.h>

#include <map>
#include <list>
#include <stdexcept>
#include <fstream>
#include <vector>

using std::ostream;
using std::endl;
using std::flush;
using std::vector;
using std::string;
using std::map;
using std::list;
using std::set;
using std::pair;
using std::FILE;

// Need to distinguish between errors that delete the model
// and errors that don't delete the model (in update)
// so that we can then dump parameter values from parser.cc

namespace jags {

Console::Console(ostream &out, ostream &err)
  : _out(out), _err(err), _model(nullptr),
    _pdata(nullptr), _prelations(nullptr),  _pvariables(nullptr)
{
}

Console::~Console()
{
    delete _model;
    delete _pdata;
    delete _prelations;
    if (_pvariables) {
	for (unsigned int i = 0; i < _pvariables->size(); ++i) {
	    delete (*_pvariables)[i];
	}
	delete _pvariables;
    }
}

void Console::handle(bool clear) {

    try {
	throw;
    }
    catch (NodeError const &except) {
	except.printMessage(_err, _model->symtab());
    }
    catch (std::runtime_error const &except) {
	_err << "RUNTIME ERROR:\n";
	_err << except.what() << endl;
    }
    catch (std::logic_error const &except) {
	_err << "LOGIC ERROR:\n" << except.what() << '\n';
	_err << "Please send a bug report to "
	     << PACKAGE_BUGREPORT << endl;
    }
    catch(...) {
	_err << "Uncaught exception\n";
	_err << "Please send a bug report to "
	     << PACKAGE_BUGREPORT << endl;
    }

    if (clear) clearModel();
}

static void getVariableNames(ParseTree const *ptree, set<string> &nameset,
			     vector<string> &namelist, 
			     vector<string> &counters)
{
    /* 
       Get variables from model, ensuring that we ignore counters.
    */

    if (ptree->treeClass() == P_VAR) {
	string const &var = ptree->name();
	if (find(counters.begin(), counters.end(), var) == counters.end()) {
	    if (nameset.count(var) == 0) {
		nameset.insert(var);
		namelist.push_back(var);
	    }
	}
    }

    vector<ParseTree*> const &param = ptree->parameters();
    for (vector<ParseTree*>::const_iterator p = param.begin(); 
	 p != param.end(); ++p) 
    {  
	//ParseTree objects of type P_BOUND can have null parameters
	if (*p == nullptr) continue;
      
	if ((*p)->treeClass() == P_FOR) {
	    ParseTree *counter = (*p)->parameters()[0];
	    for (unsigned int i = 0; i < counter->parameters().size(); ++i) {
		getVariableNames(counter->parameters()[i], nameset,
				 namelist, counters);
	    }
	    counters.push_back(counter->name());
	    getVariableNames((*p)->parameters()[1], nameset, 
			     namelist, counters);
	    counters.pop_back();
	}
	else {
	    getVariableNames(*p, nameset, namelist, counters);
	}
    }
}


bool Console::checkModel(FILE *file)
{
    if (_model) {
	_out << "Replacing existing model" << endl;
	clearModel();
    }
    _model = nullptr;

    string message;
    int status =  parse_bugs(file, _pvariables, _pdata, _prelations, message);
    if (status != 0) {
	_err << endl << "Error parsing model file:" << endl << message << endl;
	//Tidy up
	delete _pdata; _pdata = nullptr;
	delete _prelations; _prelations = nullptr;
	if (_pvariables) {
	    for (unsigned int i = 0; i < _pvariables->size(); ++i) {
		delete (*_pvariables)[i];
	    }
	    delete _pvariables; _pvariables = nullptr;
	}
	
	return false;
    }

    //Get names of all variables in the model
    set<string> nameset;
    vector<string> counterstack;
    _array_names.clear();
    if (_pvariables) {
	for (vector<ParseTree*>::const_iterator p = _pvariables->begin();
	     p != _pvariables->end(); ++p) 
	{
	    getVariableNames(*p, nameset, _array_names, counterstack);
	}
    }
    if (_pdata) {
	getVariableNames(_pdata, nameset, _array_names, counterstack);
    }
    if (_prelations) {
	getVariableNames(_prelations, nameset, _array_names, counterstack);
    }

    return true;
}


bool Console::compile(map<string, SArray> &data_table, unsigned int nchain,
                      bool gendata)
{
    if (nchain <= 0) {
	_err << "You must have at least one chain" << endl;
	return false;
    }
    if (_model) {
	_out << "Replacing existing model" << endl;
	clearModel();
    }

    RNG *datagen_rng = nullptr;
    if (_pdata && gendata) {
	_model = new BUGSModel(1);

	Compiler compiler(*_model, data_table);
	_out << "Compiling data graph" << endl;
	try {
	    if (_pvariables) {
		_out << "   Declaring variables" << endl;
		compiler.declareVariables(*_pvariables);
	    }
	    _out << "   Resolving undeclared variables" << endl;
	    compiler.undeclaredVariables(_pdata);
	    _out << "   Allocating nodes" << endl;
	    compiler.writeRelations(_pdata);
      
	    /* Check validity of data generating model */
	    for (vector<StochasticNode*>::const_iterator r = _model->stochasticNodes().begin();
		 r != _model->stochasticNodes().end(); ++r)
	    {
		if (isObserved(*r)) {
		    vector<Node const*> const &parents = (*r)->parents();
		    for (vector<Node const*>::const_iterator p = parents.begin();
			 p != parents.end(); ++p)
		    {
			if (!((*p)->isFixed())) {
			    _err << "Invalid data graph: observed stochastic node " 
				 << _model->symtab().getName(*r) 
				 << " has non-fixed parent " 
				 << _model->symtab().getName(*p)
				 << "\n";
			    clearModel();
			    return false;
			}
		    }
		}
	    }
	    _out << "   Initializing" << endl;
	    _model->initialize(true, 1U, 1U);
	    // Do a single update (by forward sampling)
	    _model->update(1);
	    //Save data generating RNG for later use. It is owned by the
	    //RNGFactory, not the model.
	    datagen_rng = _model->rng(0);
	    _out << "   Reading data back into data table" << endl;
	    _model->symtab().readValues(data_table, 0, ALL_VALUES);
	    delete _model;
	    _model = nullptr;
	}
	catch(...) {
	    handle();
	    return false;
	}
    }

    _model = new BUGSModel(nchain);
    Compiler compiler(*_model, data_table);

    _out << "Compiling model graph" << endl;
    try {
	if (_pvariables) {
	    _out << "   Declaring variables" << endl;
	    compiler.declareVariables(*_pvariables);
	}
	if (_prelations) {
	    _out << "   Resolving undeclared variables" << endl;
	    compiler.undeclaredVariables(_prelations);
	    _out << "   Allocating nodes" << endl;
	    compiler.writeRelations(_prelations);
	}
	else {
	    _err << "Nothing to compile" << endl;
	    return false;
	}
	if (_model) {
	    unsigned int nobs = 0, npart = 0, nparam = 0;
	    vector<StochasticNode*> const &snodes =  _model->stochasticNodes();
	    for (unsigned int i = 0; i < snodes.size(); ++i) {
		if (isObserved(snodes[i])) {
		    if (isParameter(snodes[i])) {
			++npart;
		    }
		    else {
			++nobs;
		    }
		}
		else {
		    ++nparam;
		}
	    }
            _out << "Graph information:\n";
	    _out << "   Fully observed stochastic nodes: " << nobs << "\n";
	    if (npart > 0) {
		_out << "   Partly observed stochastic nodes: " << npart << "\n";
	    }
	    _out << "   Unobserved stochastic nodes: " << nparam << "\n";
	    _out << "   Total graph size: " << _model->nodes().size() << endl;

	    if (datagen_rng) {
		// Reuse the data-generation RNG, if there is one, for chain 0 
		_model->setRNG(datagen_rng, 0);
	    }
	}
	else {
	    _err << "No model" << endl;
	    return false;
	}
    }
    catch(...) {
	handle();
	return false;
    }
    
    return true;
}

bool Console::initialize(unsigned nrep_root, unsigned int nrep_internal)
{
    if (_model == nullptr) {
	_err << "Can't initialize. No model!" << endl;
	return false;
    }
    if (_model->nodes().size() == 0) {
	_err << "Can't initialize. No nodes in graph." << endl;
	_err << "Have you compiled the model?" << endl;
	return false;
    }
    if (_model->isInitialized()) {
	_out << "Model is already initialized" << endl;
	return true;
    }
    
    try {
	_out << "Initializing model" << endl;
	_model->initialize(false, nrep_root, nrep_internal);
    }
    catch(...) {
	handle();
	return false;
    }
    
    return true;
}

bool Console::setParameters(map<string, SArray> const &init_table,
			    unsigned int chain)
{
  if (_model == nullptr) {
    _err << "Can't set initial values. No model!" << endl;    
    return false;
  }
  if (chain == 0 || chain > nchain()) {
    _err << "Invalid chain number" << endl;
    return false;
  }

  try {
    _model->setParameters(init_table, chain - 1);
  }
  catch(...) {
      handle();
      return false;
  }
	
  return true;
}

bool Console::setRNGname(string const &name, unsigned int chain)
{
    if (_model == nullptr) {
	_err << "Can't set RNG name. No model!" << endl;    
	return false;
    }
    try {
	bool name_ok = _model->setRNG(name, chain - 1);
	if (!name_ok) {
	    _err << "RNG name " << name << " not found\n";
	    return false;
	}
    }
    catch(...) {
	handle();
	return false;
    }
	
    return true;
}

bool Console::update(unsigned int n)
{
    if (_model == nullptr) {
	_err << "Can't update. No model!" << endl;    
	return false;
    }
    if (!_model->isInitialized()) {
	_err << "Model not initialized" << endl;
	return false;
    }
    try {
	_model->update(n);
    }
    catch(...) {
	handle(false);
	return false;
    }

    return true;
}

unsigned int Console::iter() const
{
  if (!_model) {
    return 0;
  }
  else {
    return _model->iteration();
  }
}

bool Console::setMonitor(string const &name, Range const &range,
			 unsigned int thin, string const &stat,
			 string const &summary)
{
    if (!_model) {
	_err << "Can't set monitor. No model!" << endl;    
	return false;
    }
    if (thin == 0) {
	_err << "Failed to set " << stat << " " << summary << " monitor for "
	     <<  name << printRange(range) << endl;
	_err << "Thinning interval must be > 0" << endl;
	return false;
    }

    try {

	if (_model->isAdapting()) {
	    _out << "NOTE: Stopping adaptation\n" << endl;
	    _model->adaptOff();
	}
	string msg;
	bool ok = _model->setMonitor(name, range, thin, stat, summary, msg);
	if (!ok) {
	    _err << "Failed to set " << stat << " " << summary  <<
		" monitor for " << name << printRange(range) << endl;
	    if (!msg.empty()) {
		_err << msg << endl;
	    }
	    return false;
	}
    }
    catch(...) {
	handle();
	return false;
    }

    return true;
}

bool Console::clearMonitor(string const &name, Range const &range,
			   string const &stat, string const &summary)
{
  if (!_model) {
    _err << "Can't clear monitor. No model!" << endl;    
    return false;
  }

  try {
      bool ok = _model->deleteMonitor(name, range, stat, summary);      
      if (!ok) {
	  _err << "Failed to clear " << stat << " " << summary  <<
	      " monitor for node " << name << printRange(range) << endl;
	  return false;
      }
  }
  catch(...) {
      handle();
      return false;
  }

  return true;
}

void Console::clearModel()
{
    _out << "Deleting model" << endl;
    delete _model; _model = nullptr;
}

bool Console::dumpState(map<string,SArray> &data_table,
			string &rng_name,
			ValueType type, unsigned int chain)
{
  if (!_model) {
    _err << "No model" << endl;
    return false;
  }
  if (_model->symtab().size() == 0) {
    _err << "Symbol table is empty" << endl;
    return false;
  }
  if (chain == 0 || chain > nchain()) {
    _err << "Invalid chain number" << endl;
    return false;
  }

  try {
      _model->symtab().readValues(data_table, chain - 1, type);
      
    if (type == PARAMETER_VALUES || type == ALL_VALUES) {
      
      vector<int> rngstate;
      if (_model->rng(chain - 1)) {
	_model->rng(chain - 1)->getState(rngstate);
	
	vector<unsigned long> dimrng(1,rngstate.size());
	SArray rngsarray(dimrng);

	vector<double> v(rngstate.size());
	copy(rngstate.begin(), rngstate.end(), v.begin());
	rngsarray.setValue(v);
	
	data_table.insert(pair<string, SArray>(".RNG.state",rngsarray));
	rng_name = _model->rng(chain - 1)->name();
      }
    }
  }
  catch(...) {
      handle();
      return false;
  }
  // Model is subsequently cleared by calling functions in parser
  
  return true;
}


bool Console::dumpMonitors(map<string,SArray> &data_table,
			   string const &stat,
			   string const &summary,
			   bool flat) 
{
    if (_model == nullptr) {
	_err << "Cannot dump monitors.  No model!" << endl;
	return false;
    }
    try {
	list<MonitorControl> const &monitors = _model->monitors();
	for (auto p = monitors.begin(); p != monitors.end(); ++p) {
	    Monitor const *monitor = p->monitor();
	    if (p->niter() > 0 &&
		p->stat() == stat &&
		p->summary() == summary)
	    {
		string name = p->name() + printRange(p->range());
		data_table.insert(pair<string,SArray>(name, 
						      monitor->dump(flat)));
	    }
	}
    }
    catch(...) {
	handle();
	return false;
    }

    return true;
}

void Console::dumpNodeNames(vector<string> &node_names,
			    string const &type) const
{
    if (_model == nullptr) {
	_err << "Cannot dump node names.  No model!" << endl;
	return;
    }
	
    string warn;
    _model->dumpNodeNames(node_names, type, warn);
    if (!warn.empty()) {
        _err << "WARNING:\n" << warn;
    }	
}
		 
bool Console::coda(string const &prefix, string const &stat,
		   string const &summary)
{
    if (!_model) {
	_err << "Can't dump CODA output. No model!" << endl;
	return false;
    }

    try {
        string warn;
	_model->coda(prefix, warn, stat, summary);
        if (!warn.empty()) {
            _err << "WARNING:\n" << warn;
        }
    }
    catch(...) {
	handle();
	return false;
    }

    return true;
}

bool Console::coda(vector<pair<string, Range> > const &nodes,
		   string const &prefix, string const &stat,
		   string const &summary)
{
    if (!_model) {
	_err << "Can't dump CODA output. No model!" << endl;
	return false;
    }

    try {
        string warn;
	_model->coda(nodes, prefix, warn, stat, summary);
        if (!warn.empty()) {
            _err << "WARNINGS:\n" << warn;
        }
    }
    catch(...) {
	handle();
	return false;
    }

    return true;
}

BUGSModel const *Console::model()
{
  return _model;
}

unsigned int Console::nchain() const
{
  if (_model == nullptr) {
    return 0;
  }
  else {
    return _model->nchain();
  }
}

unsigned int Console::nthread() const
{
  if (_model == nullptr) {
    return 0;
  }
  else {
    return _model->nthread();
  }
}

bool Console::checkAdaptation(bool &status) 
{
    if (_model == nullptr) {
	_err << "Can't update. No model!" << endl;
	return false;
    }
    if (!_model->isInitialized()) {
	_err << "Model not initialized" << endl;
	return false;
    }
    
    try {
	status =  _model->checkAdaptation();
    }
    catch(...) {
	handle();
	return false;
    }

    return true;
}
    
bool Console::adaptOff(void) 
{
  if (_model == nullptr) {
    _err << "Cannot stop adaptation. No model!" << endl;
    return false;
  }
  if (!_model->isInitialized()) {
    _err << "Cannot stop adaptation. Model not initialized" << endl;
    return false;
  }

  try {
      _model->adaptOff();
  }
  catch(...) {
      handle();
      return false;
  }

  return true;
}

bool Console::isAdapting() const
{
    return _model ? _model->isAdapting() : false;
}

vector<string> const &Console::variableNames() const
{
    return _array_names;
}

bool Console::dumpSamplers(vector<vector<string> > &sampler_names)
{
    if (_model == nullptr) {
	_err << "Can't dump samplers. No model!" << endl;    
	return false;
    }
    if (!_model->isInitialized()) {
	_err << "Model not initialized" << endl;
	return false;
    }

    try {
	_model->samplerNames(sampler_names);
    }
    catch(...) {
	handle();
	return false;
    }

    return true;
}

bool Console::loadModule(string const &name)
{
    list<Module*>::const_iterator p;
    for (p = Module::modules().begin(); p != Module::modules().end(); ++p)
    {
	if ((*p)->name() == name) {
	    (*p)->load();
	    if (rngSeed() != 0) {
		// Set default seed of RNG factories
		vector<RNGFactory*> const &facs = (*p)->rngFactories();
		for (unsigned int i = 0; i < facs.size(); ++i) {
		    facs[i]->setSeed(rngSeed());
		}
	    }
	    return true;
	}
    }
    return false;
}

vector<string> Console::listModules()
{
    vector<string> ans;
    for (auto p = Module::loadedModules().begin(); 
	 p != Module::loadedModules().end(); ++p)
    {
	ans.push_back((*p)->name());
    }
    return ans;
}

bool Console::unloadModule(string const &name)
{
    for (auto p = Module::loadedModules().begin(); 
	 p !=  Module::loadedModules().end(); ++p)
    {
	if ((*p)->name() == name) {
	    (*p)->unload();
	    return true;
	}
    }
    return false;
}

template<typename T>
bool setactive(string const &name, bool flag, list<T*> const &faclist)
{
    bool ans = false;
    for (auto p = faclist.begin(); p != faclist.end(); ++p) {
	if ((*p)->name() == name) {
	    (*p)->setActive(flag);
	    ans = true;
	}
    }
    return ans;
}

bool Console::setFactoryActive(string const &name, FactoryType type, bool flag)
{
    bool ok = false;
    switch(type) {
    case SAMPLER_FACTORY:
	ok = setactive<SamplerFactory>(name, flag, Model::samplerFactories());
	break;
    case MONITOR_FACTORY:
	ok = setactive<MonitorFactory>(name, flag, Model::monitorFactories());
	break;
    case RNG_FACTORY:
	ok = setactive<RNGFactory>(name, flag, Model::rngFactories());
	break;
    }
    return ok;
}

template<class T>
vector<pair<string, bool>> listfac(list<T*> const &flist)
{
    vector<pair<string, bool>> ans;
    for (auto p = flist.begin(); p != flist.end(); ++p) {
	ans.push_back(pair<string, bool>((*p)->name(), (*p)->isActive()));
    }
    return ans;
}

vector<pair<string, bool>>  Console::listFactories(FactoryType type)
{
    vector<pair<string, bool>> ans;
    switch(type) {
    case SAMPLER_FACTORY:
	ans = listfac<SamplerFactory>(Model::samplerFactories());
	break;
    case MONITOR_FACTORY:
	ans = listfac<MonitorFactory>(Model::monitorFactories());
	break;
    case RNG_FACTORY:
	ans = listfac<RNGFactory>(Model::rngFactories());
	break;
    }
    return ans;
}

unsigned int &Console::rngSeed()
{
    static unsigned int seed = 0;
    return seed;
}
    
void Console::setRNGSeed(unsigned int seed)
{
    if (seed == 0) return;

    for (auto p = Model::rngFactories().begin(); p != Model::rngFactories().end(); 
	 ++p) 
    {
	(*p)->setSeed(seed);
    }
    rngSeed() = seed;
}

    bool Console::setNThread(unsigned int nthread)
    {
	if (_model == nullptr) {
	    _err << "Can't set number of threads. No model!" << endl;
	    return false;
	}
#ifndef _OPENMP	
	nthread = 1U;
#endif
	_model->setNThread(nthread);
	return true;


    }
    
} //namespace jags
