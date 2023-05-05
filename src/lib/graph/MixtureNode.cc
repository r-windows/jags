#include <config.h>
#include <graph/MixtureNode.h>
#include <graph/GraphMarks.h>
#include <graph/Graph.h>
#include <graph/MixTab.h>1
#include <graph/NodeError.h>

//Debugging only
//#include <util/nainf.h>

#include <utility>
#include <vector>
#include <stdexcept>

using std::vector;
using std::map;
using std::invalid_argument;
using std::logic_error;
using std::set;
using std::string;
using std::pair;


namespace jags {

    static vector<Node const *> 
    mkParents2(vector<Node const *> const &index, vector<Node const *> const &nodes)
    {
	vector<Node const *> parents = index;
	unsigned long n = 0;
	for (auto p = nodes.begin(); p != nodes.end(); ++p) {
	    if (*p != nullptr) {
		parents.push_back(*p);
		n++;
	    }
	}

	if (n < 2) {
	    throw invalid_argument("Trivial MixMap in MixtureNode constructor");
	}
	
	return parents;
    }

    MixtureNode::MixtureNode(vector<Node const *> const &index,
			     unsigned int nchain, MixTab const &mixtab)
	: DeterministicNode(mixtab.dim(), nchain, mkParents2(index, mixtab.nodes())),
	  _table(mixtab), _nindex(index.size()), _discrete(true),
	  _active_parents(nchain)
    {
	// Check validity of index argument

	if (index.empty())
	    throw invalid_argument("NULL index in MixtureNode constructor");

	for (auto i = index.begin(); i != index.end(); ++i) {
	    Node const *node = *i;
	    if (node->length() != 1 || !node->isDiscreteValued() ||
		node->isFixed()) 
	    {
		throw invalid_argument("Invalid index in MixtureNode constructor");
	    }
	}

	/*
	// Check validity of MixMap argument
	if (mixmap.size() < 2)
	throw invalid_argument("Trivial MixMap in MixtureNode constructor");
	
	// Check consistency of arguments
	if (index.size() != mixmap.begin()->first.size()) {
	throw invalid_argument("Dimension mismatch in MixtureNode constructor");
	}
	*/

	//Check discreteness 
	vector<Node const *> const &par = parents();
	for (unsigned long i = _nindex; i < par.size(); ++i)
	{
	    if (!par[i]->isDiscreteValued()) {
		_discrete = false;
		break;
	    }
	}
    }

/* Do not delete commented sections: they are useful for debugging
#include <iostream>
#include <sarray/Range.h>
#include <util/nainf.h>
#include <graph/NodeError.h>
*/

void MixtureNode::updateActive(unsigned int chain)
{
    vector<unsigned long> i(_nindex);
    vector <Node const*> const &par = parents();
    for (unsigned long j = 0; j < _nindex; ++j) {
	i[j] = static_cast<unsigned long>(*par[j]->value(chain));
    }

    _active_parents[chain] = _table.getNode(i);
    if (_active_parents[chain] == nullptr) {
	/*
	std::cout << "Got " << printIndex(i) << "\nOriginally\n";
	for (unsigned int j = 0; j < _nindex; ++j) {
	    std::cout << par[j]->value(chain)[0] << "\n";
	    if (jags_isna(par[j]->value(chain)[0]))
		std::cout << "(which is  missing)\n";
	}
	*/
	string msg = string("Invalid index ") + printIndex(i) +
	    " in mixture node";
	throw NodeError(this, msg);
    }
}

void MixtureNode::deterministicSample(unsigned int chain)
{
    updateActive(chain);
    setValue(_active_parents[chain]->value(chain), length(), chain);	
}

Node const *MixtureNode::activeParent(unsigned int chain) const
{
    return _active_parents[chain];
}

unsigned long MixtureNode::index_size() const
{
  return _nindex;
}

string MixtureNode::deparse(vector<string> const &parents) const
{
    string name = "mixture(index=[";

    vector<unsigned long> i(_nindex);
    for (unsigned long j = 0; j < _nindex; ++j) {
	if (j > 0) {
	    name.append(",");
	}
	name.append(parents[j]);
    }
    name.append("], parents= ");

    /* We can't list all possible parents in a name, since there is
       no limit on the number. So we take the first and last */
    name.append(parents[_nindex]); //first parent
    if (parents.size() > _nindex + 2) {
        name.append("...");
    }
    else {
        name.append(",");
    }
    name.append(parents.back()); //last parent
    name.append(")");
	      
    return name;
}

MixtureNode const *asMixture(Node const *node)
{
  return dynamic_cast<MixtureNode const*>(node);
}

bool isMixture(Node const *node)
{
  return dynamic_cast<MixtureNode const*>(node);
}

bool MixtureNode::hasGradient(Node const *arg) const
{
    //A Mixture nodes is not differentiable with respect to the indices.
    auto par = parents();
    for (unsigned long i = 0; i < _nindex; ++i) {
	if (arg == par[i]) {
	    return false;
	}
    }
    return true;
}

void MixtureNode::gradient(double *grad, Node const *arg,
			   unsigned int chain) const
{
    //Gradient is trivially 1 with respect to the active node only
    if (arg == _active_parents[chain]) {
	for (unsigned int i = 0; i < _length; ++i) {
	    grad[i] += 1;
	}
    }
}

bool MixtureNode::isClosed(set<Node const *> const &ancestors, 
			   ClosedFuncClass fc, bool fixed) const
{
    if (fixed)
	return false;

    //Check that none of the indices are in the ancestor set
    vector<Node const*> const &par = parents();
    for (unsigned long i = 0; i < _nindex; ++i) {
	if (ancestors.count(par[i])) {
	    return false;
	}
    }

    switch(fc) {
    case DNODE_LINEAR: case DNODE_SCALE_MIX: case DNODE_POWER:
	break;
    case DNODE_SCALE: case DNODE_ADDITIVE:
	//Only a scale or additive function if all possible parents are scale
	//or additive functions, respectively.
	for (unsigned long i = _nindex; i < par.size(); ++i) {
	    if (ancestors.count(par[i])==0)
		return false;
	}
	break;
    }
    
    return true;
}

bool MixtureNode::checkParentValues(unsigned int) const
{
    return true;
}

    /*
DeterministicNode *MixtureNode::clone(vector<Node const *> const &parents) const
{
    vector<Node const *> index(_nindex);
    vector<Node const *>::const_iterator p = parents.begin();
    for (unsigned int i = 0; i < _nindex; ++i) {
	index[i] = *p;
	++p;
    }
    
    // Find the MixMap corresponding to this node, and copy it
    MixMap mixmap = findTable(_table)->first;
    // Replace entries in the copy
    MixMap::const_iterator q = mixmap.begin();
    while (p != parents.end() && q != mixmap.end()) {
	mixmap[q->first] = *p;
	++q;
	++p;
    }
    
    return new MixtureNode(index, mixmap);
}
    */
    
bool MixtureNode::isDiscreteValued() const
{
    return _discrete;
}

    MixTab const *MixtureNode::mixTab() const
    {
	return &_table;
    }

}

//namespace jags
