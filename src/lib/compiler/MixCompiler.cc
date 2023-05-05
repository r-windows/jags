/* Compiler rules for Mixture nodes.

   Node * getMixtureNode(ParseTree const * var, Compiler *compiler);
*/

#include "MixCompiler.h"

#include <model/StochasticIndex.h>
#include <compiler/ParseTree.h>
#include <model/NodeArray.h>
#include <compiler/Compiler.h>
#include <graph/Node.h>
#include <graph/NodeError.h>
#include <sarray/SimpleRange.h>
#include <sarray/RangeIterator.h>

#include <stdexcept>
#include <string>
#include <map>
#include <cmath>

using std::vector;
using std::pair;
using std::logic_error;
using std::runtime_error;
using std::string;
using std::set;
using std::map;

namespace jags {

    /* 
       Given a vector of subset indices, this function modifies the argument
       "subsets", so that it contains a vector of pairs:
       - Index: Possible value of the variable indices
       - Range: Corresponding range of the array to take as a subset
       
       All possible values of Index are included in the vector, based on the
       the range of the array we are taking subsets from (default_range).
    */
    static void getSubsetRanges(vector<pair<vector<unsigned long>, Range>> &subsets,  
				vector<StochasticIndex> const &limits,
				SimpleRange const &default_range)
    {
	unsigned long ndim = limits.size();

	// Create upper and lower bounds
	vector<unsigned long> var_offset;
	vector<unsigned long> var_lower, var_upper;
	vector<vector<unsigned long> > scope(ndim);
	for (unsigned int j = 0; j < ndim; ++j) {
	    if (limits[j].isVariable()) {
		var_offset.push_back(j);
		var_lower.push_back(default_range.lower()[j]);
		var_upper.push_back(default_range.upper()[j]);
	    }
	    else {
		scope[j] = limits[j].fixedIndex();
	    }
	}

	SimpleRange var_range(var_lower, var_upper); //range of variable indices
	for (RangeIterator p(var_range); !p.atEnd(); p.nextLeft()) {
	    for (unsigned int k = 0; k < var_offset.size(); ++k) {
		scope[var_offset[k]] = vector<unsigned long>(1, p[k]);
	    }
	    subsets.push_back(pair<vector<unsigned long>, Range>(p, Range(scope)));
	}
    }

    static int getSubsetIndices(ParseTree const *var, Compiler *compiler,
				SimpleRange const &default_range, vector<StochasticIndex> &limits)
    {
	vector<ParseTree*> const &range_list = var->parameters();
	unsigned long ndim = default_range.ndim(false);
	if (range_list.size() != ndim) {
	    throw runtime_error("Dimension mismatch taking variable subset of " + 
				var->name());
	}

	int nvi = 0; //Count number of variable indices
	for (unsigned int i = 0; i < ndim; ++i) {
	    ParseTree const *range_element = range_list[i];
	    if (range_element->treeClass() != P_RANGE) {
		throw runtime_error("Malformed range expression");
	    }
	    
	    ParseTree const *p0;
	    vector<unsigned long> indices;
	    switch(range_element->parameters().size()) {
	    case 0:
		// Index is empty, implying the whole range 
		limits.push_back(StochasticIndex(default_range.scope()[i]));
		break;
	    case 1:
		p0 = range_element->parameters()[0];
		if(compiler->indexExpression(p0, indices)) {
		    //Fixed index
		    limits.push_back(StochasticIndex(indices));
		}
		else {
		    //Variable index
		    Node *node = compiler->getParameter(p0);
		    if (node == nullptr) {
			return -1;
		    }
		    else {
			limits.push_back(StochasticIndex(node));
			++nvi;
		    }
		}
		break;
	    default:
		throw logic_error("Invalid range expression");
	    }
	    
	    
	    //Check validity of subset index
	    StochasticIndex const &ssi = limits.back();
	    if (ssi.isVariable()) {
		Node const *node = ssi.variableIndex();
		if (!node->isDiscreteValued()) {
		    throw NodeError(node, "Continuous node used as index");
		}
		if (node->length() != 1) {
		    throw NodeError(node, "Vector node used as index");
		}
	    }
	    else {
		vector<unsigned long> const &indices = ssi.fixedIndex();
		bool ok = true;
		if (indices.empty()) {
		    ok = false;
		}
		for (unsigned int j = 0; j < indices.size(); ++j) {
		    if (indices[j] < default_range.lower()[i] || indices[j] > default_range.upper()[i]) {
			ok = false;
		    }
		}
		if (!ok) {
		    throw runtime_error("Requested invalid variable subset of " +
					var->name());
		}
	    }
	}
	return nvi;
    }

    Node * getMixtureNode(ParseTree const * var, Compiler *compiler)
    {
	if (var->treeClass() != P_VAR) {
	    throw logic_error("Expecting variable expression");
	}

	vector<StochasticIndex> limits;
	NodeArray *array = compiler->model().symtab().getVariable(var->name());
	if (array == nullptr) {
	    throw runtime_error(string("Unknown parameter: ") + var->name());
	}

	int nvi = getSubsetIndices(var, compiler, array->range(), limits);
		     
	if (nvi < 0) {
	    //Unresolved variable index
	    return nullptr;
	}
	else if (nvi == 0) {
	    //No variable indices
	    throw logic_error("Trivial mixture node");
	}

	return array->getMixture(limits, compiler->model());
    }

    /**
     * This is a diagnostic function that is called by the compiler in
     * _resolution_level 2 when a mixture node cannot be resolved.
     */
    void getMissingMixParams(ParseTree const * var, UMap &umap,
			     Compiler *compiler)
    {
	NodeArray *array = compiler->model().symtab().getVariable(var->name());
	vector<StochasticIndex> limits;
	unsigned int nvi = getSubsetIndices(var, compiler, array->range(), limits);
	if (nvi < 0) {
	    return;
	}

	vector<pair<vector<unsigned long>, Range>> ranges;  
	getSubsetRanges(ranges, limits, array->range());

	for (unsigned int i = 0; i < ranges.size(); ++i) {
	    Range const &subset_range = ranges[i].second;
	    Node *node = array->getSubset(subset_range, compiler->model());
	    if (node) continue;

	    /* Make a note of all subsets that could not be resolved. */
	    bool empty = true;
	    for (RangeIterator r(subset_range); !r.atEnd(); r.nextLeft()) {
		if (array->getSubset(SimpleRange(r,r), compiler->model())) {
		    empty = false;
		    break;
		}
	    }
	    if (empty) {
		//No elements found, so report the whole range
		//as missing, e.g. x[1:5]
		pair<string, Range> upair(var->name(), subset_range);
		if (umap.find(upair) == umap.end()) {
		    set<unsigned long> lines;
		    lines.insert(var->line());
		    umap[upair] = lines;
		}
		else {
		    umap.find(upair)->second.insert(var->line());
		}
	    }
	    else {
		//Some elements found, so report only the missing
		//elements, e.g. x[5]
		for (RangeIterator r(subset_range); !r.atEnd(); r.nextLeft()) {
		    SimpleRange sr(r,r);
		    if (!array->getSubset(sr, compiler->model())) {
			pair<string, Range> upair(var->name(), sr);
			if (umap.find(upair) == umap.end()) {
			    set<unsigned long> lines;
			    lines.insert(var->line());
			    umap[upair] = lines;
			}
			else {
			    umap.find(upair)->second.insert(var->line());
			}
		    }
		}
	    }
	}
    }    


} //namespace jags

