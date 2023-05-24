#include <config.h>
#include <model/SymTab.h>
#include <model/Model.h>
#include <graph/Node.h>
#include <util/nainf.h>
#include <util/dim.h>

#include <string>
#include <stdexcept>
#include <utility>
#include <set>
#include <sstream>

using std::vector;
using std::map;
using std::pair;
using std::string;
using std::runtime_error;
using std::logic_error;
using std::set;
using std::ostringstream;

namespace jags {

SymTab::SymTab(Model *model)
    : _model(model)
{
}

SymTab::~SymTab() {
    
    map<string, NodeArray*>::iterator p;
    for (p = _varTable.begin(); p != _varTable.end(); ++p) {
	delete p->second;
    }
}

void SymTab::addVariable(string const &name, vector<unsigned long> const &dim)
{
    if (_varTable.find(name) != _varTable.end()) {
	string msg("Name ");
	msg.append(name);
	msg.append(" already in use in symbol table");
	throw runtime_error(msg);
    }

    if (isFlat(dim)) {
	string msg = string("Cannot create variable ") + name + 
	    " with zero dimension";
	throw runtime_error(msg);
    }
    NodeArray *array = new NodeArray(name, dim, _model->nchain());
    _varTable[name] = array;
}

NodeArray* SymTab::getVariable(string const &name) const
{
  map<string, NodeArray*>::const_iterator p =  _varTable.find(name);
  
  if (p == _varTable.end()) {
    return nullptr;
  }
  else {
    return p->second;
  }
}

void SymTab::writeData(map<string, SArray> const &data_table)
{
  for(map<string, SArray>::const_iterator p(data_table.begin()); 
      p != data_table.end(); ++p) {
    NodeArray *array = getVariable(p->first);
    if (array) {
      if (array->range().dim(false) != p->second.dim(false)) {
	string msg("Dimension mismatch in values supplied for ");
	msg.append(p->first);
	throw runtime_error(msg);
      }
      array->setData(p->second, _model);
    }
  }
}


void SymTab::writeValues(map<string, SArray> const &data_table,
		         unsigned int chain)
{
    for(map<string, SArray>::const_iterator p(data_table.begin()); 
	p != data_table.end(); ++p) {
        //set<Node*> psetnodes;
	NodeArray *array = getVariable(p->first);
	if (array) {
	    if (array->range().dim(false) != p->second.dim(false)) {
		string msg("Dimension mismatch in values supplied for ");
		msg.append(p->first);
		throw runtime_error(msg);
	    }
	    //array->setValue(p->second, chain, psetnodes);
	    array->setValue(p->second, chain);
            //setnodes.insert(psetnodes.begin(), psetnodes.end());
	}
    }
}

static bool allMissing(SArray const &sarray)
{
    unsigned long N = sarray.length();
    vector<double> const &v = sarray.value();
    for (unsigned long i = 0; i < N; ++i) {
	if (!jags_isna(v[i]))
	    return false;
    }
    return true;
}

void SymTab::readValues(map<string, SArray> &data_table, 
		        unsigned int chain, ValueType type) const
{
    if (chain > _model->nchain()) 
	throw logic_error("Invalid chain in SymTab::readValues");

    map<string, NodeArray*>::const_iterator p;
    for (p = _varTable.begin(); p != _varTable.end(); ++p) {
	/* Create a new SArray to hold the values from the symbol table */
	SArray read_values(p->second->range().dim(false));
	p->second->getValue(read_values, chain, type);
	/* Only write to the data table if we can find at least one
	   non-missing value */
	if (!allMissing(read_values)) {
	    string const &name = p->first;
	    if (data_table.find(name) != data_table.end()) {
		//Replace any existing entry
		data_table.erase(name);
	    }
	    data_table.insert(pair<string,SArray>(name, read_values));
	}
    }
}

unsigned long SymTab::size() const
{
  return _varTable.size();
}

void SymTab::clear()
{
  _varTable.clear();
}

string SymTab::getName(Node const *node) const
{
    for (auto p = _varTable.begin(); p != _varTable.end(); ++p) {
	// Check for mixture nodes
	vector<StochasticIndex> srange = p->second->getStochasticRange(node);
	if (!srange.empty()) {
	    return p->first + printName(srange);
	}
    }

    for (auto p = _varTable.begin(); p != _varTable.end(); ++p) {
	NodeArray *array = p->second;
	// Check for inserted nodes and generated aggregated nodes
	Range node_range = array->getRange(node);
	if (!isNULL(node_range)) {
	    if (node_range == array->range()) {
		return p->first;
	    }
	    else {
		return p->first + printRange(array->getRange(node));
	    }
	}
    }

    
    //Name not in symbol table: calculate name from parents
    vector<Node const *> const &parents = node->parents();
    vector<string> parnames(parents.size());
    for (unsigned int i = 0; i < parents.size(); ++i) {
	parnames[i] = getName(parents[i]);
    }
    return node->deparse(parnames);
}

    void SymTab::lock() {
	map<string, NodeArray*>::iterator p;
	for (p = _varTable.begin(); p != _varTable.end(); ++p) {
	    p->second->lock();
	}


    }

    string
    SymTab::getMixtureIndex(Node const *node, unsigned long chain) const
    {
	for (auto p = _varTable.begin(); p != _varTable.end(); ++p) {
	    vector<StochasticIndex> srange = p->second->getStochasticRange(node);
	    if (!srange.empty()) {
		return printValue(srange, chain);
	    }
	}
	return "";
    }

    
    string
    SymTab::printName(vector<StochasticIndex> const &srange) const
    {
	if (srange.empty())
	    return "";
	
	ostringstream ostr;
	ostr << "[";
	for (auto p = srange.begin(); p != srange.end(); ++p) {
	    if (p != srange.begin()) {
		ostr << ",";
	    }
	    if (p->isVariable()) {
		ostr << getName(p->variableIndex());
	    }
	    else {
		vector<unsigned long> const &index = p->fixedIndex();
		ostr << index.front();
		if (index.size() > 1) {
		    bool simple = true;
		    for (unsigned long j = 1; j < index.size(); ++j) {
			if (index[j] != index[j-1] + 1) {
			    simple = false;
			    break;
			}
		    }
		    if (simple) {
			ostr << ":";
		    }
		    else {
			ostr << "...";
		    }
		    ostr << index.back();
		}
	    }
	}
	ostr << "]";
	return ostr.str();
    }

    string SymTab::printValue(vector<StochasticIndex> const &srange,
			      unsigned int chain) const
    {
	ostringstream ostr;
	ostr << "[";
	for (auto p = srange.begin(); p != srange.end(); ++p) {
	    if (p != srange.begin()) {
		ostr << ",";
	    }
	    if (p->isVariable()) {
		Node const *node = p->variableIndex();
		ostr << node->value(chain)[0];
	    }
	    else {
		vector<unsigned long> const &index = p->fixedIndex();
		ostr << index.front();
		if (index.size() > 1) {
		    bool simple = true;
		    for (unsigned long j = 1; j < index.size(); ++j) {
			if (index[j] != index[j-1] + 1) {
			    simple = false;
			    break;
			}
		    }
		    if (simple) {
			ostr << ":";
		    }
		    else {
			ostr << "...";
		    }
		    ostr << index.back();
		}
	    }
	}
	ostr << "]";
	return ostr.str();
    }


    
} //namespace jags
