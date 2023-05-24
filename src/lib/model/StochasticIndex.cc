#include <model/StochasticIndex.h>
#include <graph/Node.h>
#include <model/SymTab.h>
#include <util/dim.h>

#include <stdexcept>

using std::string;
using std::vector;
using std::logic_error;
using std::to_string;

static bool isSimple(vector<unsigned long> const &index)
{
    for (unsigned long i = 1; i < index.size(); ++i) {
	if (index[i] != index[i-1] + 1) return false;
    }
    return true;
}

namespace jags {

    const vector<unsigned long> StochasticIndex::_null_fixed =
	vector<unsigned long>();
    
    StochasticIndex::StochasticIndex(Node const *node)
	: _variable(node), _fixed(_null_fixed)
    {
	if (!isValid(node)) throw logic_error("Invalid StochasticIndex");
    }

    StochasticIndex::StochasticIndex(vector<unsigned long> const &fixed)
	: _variable(nullptr), _fixed(getUnique(fixed))
    {
	if (fixed.empty()) throw logic_error("Invalid StochasticIndex");
    }

    Node const *StochasticIndex::variableIndex() const
    {
	return _variable;
    }

    vector<unsigned long> const &StochasticIndex::fixedIndex() const
    {
	return _fixed;
    }
    
    string StochasticIndex::deparse(SymTab const &symtab) const
    {
	if (_variable == nullptr) {
	    unsigned long N = _fixed.size();
	    string name;
	    if (N == 1) {
		name = to_string(_fixed[0]);
	    }
	    else if (isSimple(_fixed)) {
		name = to_string(_fixed[0]) + ":" + to_string(_fixed[N-1]);
	    }
	    else {
		name = string("c(") + to_string(_fixed[0]) + ",";
		if (N==2) {
		    name += to_string(_fixed[1]);
		}
		else if (N==3) {
		    name += to_string(_fixed[1]) + "," + to_string(_fixed[2]);
		}
		else {
		    name += "...," + to_string(_fixed[N-1]);
		}
		name += ")";
	    }
	    return name;
	}
	else {
	    return symtab.getName(_variable);
	}
    }

    bool StochasticIndex::isValid(Node const *node)
    {
	if (node == nullptr) {
	    return false;
	}
	else if (node->isFixed()) {
	    return false;
	}
	else if (node->length() != 1) {
	    //FIXME We can relax this
	    return false;
	}
	return true;
    }

    bool StochasticIndex::isVariable() const
    {
	return _variable != nullptr;
    }
    
    bool lt(StochasticIndex const &lhs, StochasticIndex const &rhs)
    {
	if (lhs.variableIndex() < rhs.variableIndex()) {
	    return true;
	}
	else if (rhs.variableIndex() < lhs.variableIndex()) {
	    return false;
	}
	else if (lhs.fixedIndex() < rhs.fixedIndex()) {
	    return true;
	}
	else {
	    return false;
	}
    }

    bool
    lt(vector<StochasticIndex> const &lhs, vector<StochasticIndex> const &rhs)
    {
	if (lhs.size() < rhs.size()) {
	    return true;
	}
	else if (rhs.size() < lhs.size()) {
	    return false;
	}
	else {
	    for (unsigned long i = 0; i < lhs.size(); ++i) {
		if (lt(lhs[i], rhs[i]))
		    return true;
		else if (lt(rhs[i], lhs[i]))
		    return false;
	    }
	}
	return false;
    }



}
