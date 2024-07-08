#include <config.h>

#include <compiler/Compiler.h>
#include <compiler/ParseTree.h>
#include <graph/ScalarLogicalNode.h>
#include <graph/VectorLogicalNode.h>
#include <graph/ArrayLogicalNode.h>
#include <graph/LinkNode.h>
#include <graph/ConstantNode.h>
#include <graph/ScalarStochasticNode.h>
#include <graph/VectorStochasticNode.h>
#include <graph/ArrayStochasticNode.h>
#include <graph/AggNode.h>
#include <graph/NodeError.h>
#include <sarray/SimpleRange.h>
#include <sarray/RangeIterator.h>
#include <function/FunctionPtr.h>
#include <distribution/DistPtr.h>
#include <util/nainf.h>
#include <util/dim.h>
#include <util/integer.h>

#include "MixCompiler.h"

#include <utility>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <list>
#include <algorithm>
#include <string>
#include <set>
#include <sstream>
#include <map>

using std::string;
using std::vector;
using std::list;
using std::map;
using std::pair;
using std::invalid_argument;
using std::runtime_error;
using std::logic_error;
using std::ostringstream;
using std::min;
using std::max;
using std::set;
using std::fabs;
using std::max_element;

namespace jags {

    static inline bool isLocked(NodeArray const *array)
    {
	/* Safe version of NodeArray::isLocked which returns false
	   when given a NULL pointer */
	return array ? array->isLocked() : false;
    }
    
    static SimpleRange const &rangeLocked(NodeArray const *array)
    {
	/* Version of NodeArray::range which returns a reference to
	   a NULL range if the array is not locked */
	static const SimpleRange _null;
	return isLocked(array) ? array->range() : _null;
    }
    
    static bool emptyRange(ParseTree const *var)
    {
	if (var->treeClass() != P_VAR) {
	    throw logic_error("Expecting variable expression");
	}
	vector<ParseTree*> const &range_list = var->parameters();
	return range_list.empty();
    }
    
    typedef pair<vector<unsigned long>, vector<double> > CNodeKey;

    /*
    static bool lt(CNodeKey const &k1, CNodeKey const &k2) {
	//Sort first on dimension
	if (k1.first < k2.first) {
	    return true;
	}
	else if (k1.first > k2.first) {
	    return false;
	}
	else {
	    //Fuzzy sort on values
	    return lt(&k1.second[0], &k2.second[0], k1.second.size());
	}

    }
    */
    typedef map<CNodeKey, ConstantNode *, fuzzy_less<CNodeKey> > CNodeMap;

#include <sstream>
template<class T> 
string ToString(const T& val)
{
    ostringstream strm;
    strm << val;
    return strm.str();
}

[[noreturn]] static void CompileError(ParseTree const *p, string const &msg1, 
				      string const &msg2 = "")
{
    string msg = string("Compilation error on line ") + ToString(p->line())
	+ ".";
    if (!msg1.empty()) {
	msg.append("\n");
	msg.append(msg1);
    }
    if (!msg2.empty()) {
	msg.append(" ");
	msg.append(msg2);
    }
    throw runtime_error(msg);
}

void Compiler::getLHSVars(ParseTree const *relations)
{
    /* 
       Utility function to get the names of variables used on the
       left hand side of a relation. These are added to the set
       _lhs_vars. The main use of this information is to give better
       diagnostic messages when we cannot resolve a variable name.

       Although this function has a similar recursive structure to
       traverseBackwards it expands for loops without defining a counter.
    */
    
    vector<ParseTree*> const &relation_list = relations->parameters();
    for (vector<ParseTree*>::const_reverse_iterator p = relation_list.rbegin(); 
	 p != relation_list.rend(); ++p) 
    {
	TreeClass tc = (*p)->treeClass();
	if (tc == P_STOCHREL || tc == P_DETRMREL) {
	    ParseTree *var = (*p)->parameters()[0];
	    _lhs_vars.insert(var->name());
	}
	else if (tc == P_FOR) {
	    getLHSVars((*p)->parameters()[1]);
	}
    }
}

Node * Compiler::constFromTable(ParseTree const *p)
{
    /* Get a constant value directly from the data table */

    if (!_index_expression) {
	throw logic_error("Can only call constFromTable inside index expression");
    }
    
    if (_countertab.getCounter(p->name())) {
	// DO NOT get value from the data table if there is a counter
	// that shares the same name.
	return nullptr;
    }
    map<string,SArray>::const_iterator i = _data_table.find(p->name());
    if (i == _data_table.end()) {
	//No entry in data table with this name
	return nullptr;
    }
    SArray const &sarray = i->second;

    Range subset_range = getRange(p, sarray.range());
    if (isNULL(subset_range)) {
	//Range expression not evaluated
	return nullptr;
    }
    else if (!sarray.range().contains(subset_range)) {
	//Out of range. This is almost certainly an error, but we are
	//conservative here and just return a null pointer.
	return nullptr;
    }
    else if (subset_range.length() > 1) {
	//Multivariate constant
	RangeIterator r(subset_range);
	unsigned long n = subset_range.length();
	vector<double> const &v = sarray.value();
	vector<double> value(n);
	for (unsigned long j = 0; j < n; ++j, r.nextLeft()) {
	    unsigned long offset = sarray.range().leftOffset(r);
	    value[j] = v[offset];
	    if (jags_isna(value[j])) {
		return nullptr;
	    }
	}
	return getConstant(subset_range.dim(false), value, 
			   _model.nchain(), true);
    }
    else {
	//Scalar constant
	unsigned long offset = sarray.range().leftOffset(subset_range.first());  
	double value = sarray.value()[offset];
	if (jags_isna(value)) {
	    return nullptr;
	}
	else {
	    return getConstant(value, _model.nchain(), true);
	}
    }
}

bool Compiler::indexExpression(ParseTree const *p, vector<unsigned long> &value)
{
    /* 
       Evaluates an index expression.

       Index expressions occur in three contexts:
       1) In the limits of a "for" loop
       2) On the left hand side of a relation
       3) On the right hand side of a relation
     
       They are integer-valued, constant expressions.  We return true
       on success and the result is written to the parameter value.
    */
    
    /* 
       The counter _index_expression is non-zero if we are inside an
       Index expression. This invokes special rules in the functions
       getParameter and getArraySubset.  The counter tracks the levels
       of nesting of index expressions.
       
       The vector _index_nodes holds the Nodes created during the 
       evaluation of the index expression.
    */

    bool isfixed = true;

    _index_expression++;
    Node *node = getParameter(p);
    _index_expression--;

    if (!node || !node->isFixed()) {
	isfixed = false;
    }
    else {
	for (unsigned int i = 0; i < node->length(); ++i) {
	    double v = node->value(0)[i];
	    if (!checkInteger(v)) {
		throw NodeError(node, 
				"Index expression not an integer.");
	    }
	    value.push_back(asULong(v));
	}
    }

    /* Make sure we always clean up at the top-level call to IndexExpression */

    if (_index_expression == 0) {
	while(!_index_nodes.empty()) {
	    Node *inode = _index_nodes.back();
	    _index_nodes.pop_back();
	    inode->unlinkParents();
	    delete inode;
	}
    }

    return isfixed;
}

Range Compiler::getRange(ParseTree const *p, 
			 SimpleRange const &default_range)
{
    /* 
       Evaluate a range expression. If successful, it returns the range
       corresponding to the expression.  If unsuccessful (due to missing
       values) returns a null range.
     
       The default_range argument provides default values if the range
       expression is blank: e.g. foo[] or bar[,1].  The default range 
       may be a NULL range, in which case, missing indices will result in
       failure.
    */
  
    vector<ParseTree*> const &range_list = p->parameters();
    if (range_list.empty()) {
	//An empty range expression implies the default range
	return default_range;
    }

    // Check size of range expression
    unsigned long size = range_list.size();
    if (!isNULL(default_range) && size != default_range.ndim(false)) {
	CompileError(p, "Dimension mismatch taking subset of", p->name());
    }

    // Now step through and evaluate index expressions
    vector<vector<unsigned long>> scope(size);
    for (unsigned int i = 0; i < size; i++) {
	if (range_list[i]->treeClass() == P_NULL) {
	    // Empty index implies default range
	    if (isNULL(default_range)) {
		return Range();
	    }
	    scope[i] = default_range.scope()[i];
	}
	else{
	    if (!indexExpression(range_list[i], scope[i])) {
		return Range();
	    }
	    if (scope[i].empty()) {
		CompileError(p, "Invalid range");
	    }
	}
    }

    return Range(scope);
}

SimpleRange Compiler::VariableSubsetRange(ParseTree const *var)
{
    /*
      Get the range of a subset expression of a variable on the LHS of a
      relation.  This means that the subset expression must be constant,
      and it must be a simple range. It can only have missing indices
      if the array is declared.
    */
    if (var->treeClass() != P_VAR) {
	throw logic_error("Expecting variable expression");
    }  

    string const &name = var->name();
    if (_countertab.getCounter(name)) {
	CompileError(var, "Attempt to redefine counter inside a for loop:",
		     name);
    }
    NodeArray *array = _model.symtab().getVariable(name);
    vector<ParseTree*> const &range_list = var->parameters();
    if (isLocked(array)) {
	if (range_list.empty()) {
	    //Missing range implies the whole node
	    return array->range();
	}
	if (range_list.size() != array->range().ndim(false)) {
	    CompileError(var, "Dimension mismatch in subset expression of",
			 name);
	}
    }
    else {
	// Check for empty indices
	for (auto p = range_list.begin(); p != range_list.end(); ++p) {
	    if ((*p)->treeClass() == P_NULL) {
		CompileError(*p, "Missing index in subset expression of",
			     name);
	    }
	}
    }

    Range range = getRange(var, rangeLocked(array));
    if (isNULL(range)) {
	return SimpleRange();
    }
    
    //Enforce use of simple ranges on the LHS of a relation
    for (unsigned int i = 0; i < range.ndim(false); ++i) {
	vector<unsigned long> const &indices = range.scope()[i];
	unsigned long j = indices[0];
	for (auto p = indices.begin(); p != indices.end(); ++p)
	{
	    if (*p != j++) {
		string msg = string("Invalid subset expression for ") + name +
		    "\nIndex expressions on the left hand side of a relation"
		    + "\nmust define a contiguous, increasing set of indices";
		CompileError(var, msg);
	    }
	}
    }
    return SimpleRange(range.first(), range.last());
}
 
std::vector<unsigned long> Compiler::CounterRange(ParseTree const *var)
{
    /* The range expression for a counter differs from that of
       a variable in that it is
       1) one-dimensional
       2) may not be empty
       Further, no variables are created for counters in the
       Symbol Table
    */
    if (var->treeClass() != P_COUNTER) {
	throw logic_error("Expecting counter expression");
    }
    if (var->parameters().size() != 1) {
	throw logic_error("Invalid counter expression");
    }
  
    ParseTree const *prange = var->parameters()[0];
    vector<unsigned long> indices;
    if(!indexExpression(prange, indices)) {
	CompileError(var, "Cannot evaluate range of counter", var->name());
    }

    return indices;
}

    Node * 
    Compiler::getConstant(double value, unsigned int nchain, bool observed) 
    {
	return getConstant(vector<unsigned long>(1, 1), vector<double>(1, value),
			   nchain, observed);
    }

    Node * 
    Compiler::getConstant(vector<unsigned long> const &dim, 
			  vector<double> const &value,
			  unsigned int nchain, bool observed)
    {
	ConstantNode * cnode = nullptr;
	if (_index_expression) {
	    cnode = new ConstantNode(dim, value, nchain, observed);
	    _index_nodes.push_back(cnode);
	}
	else {
	    CNodeMap::key_type k(dim, value);
	    CNodeMap::const_iterator p = _cnode_map.find(k);
	    if (p != _cnode_map.end()) {
		cnode = p->second;
	    }
	    else {
		cnode = new ConstantNode(dim, value, nchain, observed);
		_cnode_map[k] = cnode;
		_model.addNode(cnode);
	    }
	}
	return cnode;
    }

Node *Compiler::getArraySubset(ParseTree const *p)
{
    Node *node = nullptr;
    
    if (p->treeClass() != P_VAR) {
	throw logic_error("Expecting expression");
    }

    if (_index_expression) {
	//It is possible to evaluate an index expression before
	//any Nodes are available from the symbol table.
	node = constFromTable(p);
	if (node) return node;
    }
    
    Counter *counter = _countertab.getCounter(p->name()); //A counter
    if (counter) {
	node = getConstant(counter->value(), _model.nchain(), false);
    }
    else {
	NodeArray *array = _model.symtab().getVariable(p->name());
	if (array) {
	    Range subset_range = getRange(p, rangeLocked(array));
	    if (!isNULL(subset_range)) {
		//A fixed subset
		if (array->isLocked() && !array->range().contains(subset_range)) {
		    CompileError(p, "Subset out of range:", array->name() +
				 printRange(subset_range));
		}
		node = array->getSubset(subset_range, _model);
		if (node == nullptr && _compiler_mode == COLLECT_UNRESOLVED) {
		    /* Nake a note of all subsets that could not be
		       resolved.

		       Example: we have just failed to resolve x[1:5]
		       but we need to know which of x[1], x[2], x[3],
		       x[4], x[5] are empty.
		    */
		    bool empty = true;
		    for (RangeIterator r(subset_range); !r.atEnd();
			 r.nextLeft())
		    {
			if (array->getSubset(SimpleRange(r,r), _model)) {
			    empty = false;
			    break;
			}
		    }
		    if (empty) {
			//No elements found, so report the whole range
			//as missing, e.g. x[1:5]
			pair<string, Range> upair(p->name(), subset_range);
			if (_umap.find(upair) == _umap.end()) {
			    set<unsigned long> lines;
			    lines.insert(p->line());
			    _umap[upair] = lines;
			}
			else {
			    _umap.find(upair)->second.insert(p->line());
			}
		    }
		    else {
			//Some elements found, so report only the missing
			//elements, e.g. x[5]
			for (RangeIterator r(subset_range); !r.atEnd();
			     r.nextLeft())
			{
			    SimpleRange sr(r,r);
			    if (!array->getSubset(sr, _model)) {
				pair<string, Range> upair(p->name(), sr);
				if (_umap.find(upair) == _umap.end()) {
				    set<unsigned long> lines;
				    lines.insert(p->line());
				    _umap[upair] = lines;
				}
				else {
				    _umap.find(upair)->second.insert(p->line());
				}
			    }
			}
		    }
		}
	    }
	    else if (array->isLocked() && !_index_expression) {
		//A stochastic subset
		node = getMixtureNode(p, this);
		if (node == nullptr && _compiler_mode == COLLECT_UNRESOLVED) {
		    getMissingMixParams(p, _umap, this);
		}
	    } 
	}
	else if (_lhs_vars.find(p->name()) == _lhs_vars.end()) {
	    string msg = string("Unknown variable ") + p->name() + "\n" +
		"Either supply values for this variable with the data\n" +
		"or define it  on the left hand side of a relation.";
	    CompileError(p, msg);
	}
	else if (_compiler_mode == ENFORCING) {
	    string msg = string("Possible directed cycle involving variable `")
		+ p->name() + "` ";
	    CompileError(p, msg);
	}
    }
    return node;
}

static FunctionPtr const &
getFunction(ParseTree const *t, FuncTab const &functab)
{
    if (t->treeClass() != P_FUNCTION) 
	throw logic_error("Malformed parse tree: Expected function");

    FunctionPtr const &func = functab.find(t->name());
    if (isNULL(func)) {
	CompileError(t, "Unknown function:", t->name());
    }    

    return func;
}

Node *Compiler::evalBuiltinFunction(ParseTree const *p, SymTab const &symtab)
{
    /*
     * The functions length(), dim(), nrow(), ncol() are built into
     * the Compiler. The values of these functions do not depend on
     * the values of the arguments but only their size or dimension.
     * 
     * When one of these functions is evaluated by the Compiler a new
     * ConstantNode is generated. It is not appropriate to create a
     * LogicalNode, as this would create redundant parent-child
     * relationships in the model graph.  Furthermore, the results of
     * these built-in functions are always fixed, whereas a
     * LogicalNode is only fixed if all parents are fixed. Hence these
     * functions must be built into the compiler and cannot be handled
     * by Function objects.
     */
    if (p->treeClass() != P_FUNCTION) {
	throw logic_error("Malformed parse tree. Expecting function call");
    }
    string const &funcname = p->name();
    if (funcname != "length" && funcname != "dim" &&
	funcname != "nrow" && funcname != "ncol")
    {
	return nullptr; //Not a built-in function
    }
    vector<ParseTree *> const &params = p->parameters();
    if (params.size() > 1) {
	//All built-in functions take a single parameter
	CompileError(p, "Too many arguments for", funcname);
    }
    else if (params.empty() || params.front()->treeClass() == P_NULL) {
	CompileError(p, "Empty argument list for", funcname);
    }
    ParseTree const *arg = params[0];
    if (arg->treeClass() == P_VAR) {
	/* 
	 * If the argument is of the form "name[range]" then we can
	 * calculate the dimensions directly from the array and the
	 * expression for the range. When data are supplied to the
	 * data table, an array of appropriate size is created before
	 * any nodes are defined. This allows the Compiler to resolve
	 * these expressions early in the compilation process.
	 */
	NodeArray const *array = symtab.getVariable(arg->name());
	if (!isLocked(array)) {
	    return nullptr;
	}
	Range subset_range = getRange(arg, array->range());
	if (isNULL(subset_range)) {
	    return nullptr;
	}
	else if (funcname == "length") {
	    double length = subset_range.length();
	    return getConstant(length, _model.nchain(), false);
	}
	else if (funcname == "dim") {
	    vector<unsigned long> idim = subset_range.dim(false);
	    vector<double> ddim(idim.begin(), idim.end());
	    
	    vector<unsigned long> d(1, subset_range.ndim(false));
	    return getConstant(d, ddim, _model.nchain(), false);
	}
	else if (funcname == "nrow" || funcname == "ncol") {
	    if (subset_range.ndim(false) != 2) {
		CompileError(p, "Incorrect number of dimensions for", funcname);
	    }
	    vector<unsigned long> idim = subset_range.dim(false);
	    if (funcname == "nrow") {
		return getConstant(idim[0], _model.nchain(), false);
	    }
	    else {
		return getConstant(idim[1], _model.nchain(), false);
	    }
	}
	else {
	    return nullptr; //Unreachable but here for -Wall
	}
    }
    else {
	/**
	 * For more complex expressions than "name[range]" we need to
	 * create a node from the function argument and read its
	 * dimensions.
	 */
	Node const *argnode = getParameter(arg);
	if (argnode == nullptr) {
	    return nullptr;
	}
	else if (funcname == "length") {
	    return getConstant(argnode->length(), _model.nchain(), false);
	}
	else if (funcname == "dim") {
	    vector<unsigned long> idim = argnode->dim();
	    vector<double> ddim(idim.begin(), idim.end());
	    
	    vector<unsigned long> d(1, idim.size());
	    return getConstant(d, ddim, _model.nchain(), false);
	}
	else if (funcname == "nrow" || funcname == "ncol") {
	    vector<unsigned long> const &idim = argnode->dim();
	    if (idim.size() != 2) {
		CompileError(p, "Incorrect number of dimensions for", funcname);
	    }
	    if (funcname == "nrow") {
		return getConstant(idim[0], _model.nchain(), false);
	    }
	    else {
		return getConstant(idim[1], _model.nchain(), false);
	    }
	}
	else {
	    return nullptr; //Unreachable but here for -Wall
	}
    }
}

/*
 * Evaluates the expression t, and returns a pointer to a Node. If the
 * expression cannot be evaluated, a NULL pointer is returned. 
 */
Node * Compiler::getParameter(ParseTree const *t)
{
    vector<Node const *> parents;
    Node *node = nullptr;

    switch (t->treeClass()) {
    case P_VALUE:
	node = getConstant(t->value(), _model.nchain(), false);
	break;
    case P_VAR:
	node = getArraySubset(t);
	break;
    case P_LINK:
	if (getParameterVector(t, parents)) {
	    LinkFunction const *link = funcTab().findLink(t->name());
	    if (!link) {
		CompileError(t, "Unknown link function:", t->name());
	    }
	    node = _logicalfactory.getNode(FunctionPtr(link), parents, _model);
	}
	break;
    case P_FUNCTION:
	node = evalBuiltinFunction(t, _model.symtab());
	if (!node && getParameterVector(t, parents)) {
	    FunctionPtr const &func = getFunction(t, funcTab());
	    if (_index_expression) {
		node = LogicalFactory::newNode(func, parents, _model.nchain());
		_index_nodes.push_back(node);
	    }
	    else {	    
		node = _logicalfactory.getNode(func, parents, _model);
	    }
	}
	break;
    case P_NULL:
	return nullptr;
    case P_DISTMOD:  case P_COUNTER: case P_DENSITY:
    case P_STOCHREL: case P_DETRMREL: case P_FOR: case P_RELATIONS:
    case P_VECTOR: case P_ARRAY: case P_SUBSET: case P_STRUCT:
	throw  logic_error("Malformed parse tree.");
    }

    if (!node)
        return nullptr;

    if (_index_expression && !node->isFixed())
	return nullptr;

    return node;
}

static bool checkEmptyParameters(vector<ParseTree *> const &params)
{
    if (params.empty()) return false;
    for (auto p = params.begin(); p != params.end(); ++p) {
	if ((*p)->treeClass() == P_NULL) return false;
    }
    return true;
}

/*
 * Before creating the node y <- foo(a,b), or z ~ dfoo(a,b), the parent
 * nodes must a,b be created. This expression evaluates the vector(a,b)
 * Arguments are the same as for getParameter.
 */
bool Compiler::getParameterVector(ParseTree const *t,
				  vector<Node const *> &parents)
{
    if (!parents.empty()) {
	throw logic_error("parent vector must be empty in getParameterVector");
    }

    bool ok = true;
    switch (t->treeClass()) {
    case P_FUNCTION: case P_LINK: case P_DENSITY:
	if (!checkEmptyParameters(t->parameters()))
	    CompileError(t, "Parameter(s) missing for", t->name());
	for (unsigned int i = 0; i < t->parameters().size(); ++i) {
	    Node *node = getParameter(t->parameters()[i]);
	    if (node) {
		parents.push_back(node);
	    }
	    else {
		ok = false;
	    }
	}
	if (!ok) {
	    parents.clear();
	    return false;
	}
	break;
    case P_VAR: case P_DISTMOD:  case P_COUNTER: case P_VALUE:
    case P_STOCHREL: case P_DETRMREL: case P_FOR: case P_RELATIONS:
    case P_VECTOR: case P_ARRAY: case P_SUBSET: case P_STRUCT: case P_NULL:
	throw logic_error("Invalid Parse Tree.");
    }
    return true;
}

Node * Compiler::allocateStochastic(ParseTree const *stoch_relation)
{
    ParseTree const *distribution = stoch_relation->parameters()[1];  

    // Create the parameter vector
    vector<Node const *> parameters;
    if (!getParameterVector(distribution, parameters)) {
	return nullptr;
    }

    // Set upper and lower bounds
    Node *lBound = nullptr, *uBound = nullptr;
    if (stoch_relation->parameters().size() == 3) {
	//Truncated distribution
	ParseTree const *modifier = stoch_relation->parameters()[2];
	if (modifier->treeClass() != P_DISTMOD) {
	    throw logic_error("Invalid parse tree");
	}
	if (modifier->name() == "T" || modifier->name() == "I") {
	    if (modifier->parameters().size() != 2) {
		CompileError(modifier, "Incorrect number of parameters for T() or I()");
	    }
	    ParseTree const *ll = modifier->parameters()[0];
	    ParseTree const *ul = modifier->parameters()[1];
	    if (ll && ll->treeClass() != P_NULL) {
		lBound = getParameter(ll);
		if (!lBound) {
		    return nullptr;
		}
	    }
	    if (ul && ul->treeClass() != P_NULL) {
		uBound = getParameter(ul);
		if (!uBound) {
		    return nullptr;
		}
	    }
	}
	else {
	    CompileError(modifier, "Unknown modifier", modifier->name());
	}
    }

    /* 
       Check data table to see if this is an observed node.  If it is,
       we put the data in an array of doubles pointed to by this_data,
       and set data_length equal to the length of the array
    */
    double *this_data = nullptr;
    unsigned long data_length = 0;

    ParseTree *var = stoch_relation->parameters()[0];
    map<string,SArray>::const_iterator q = _data_table.find(var->name());
    if (q != _data_table.end()) {

	vector<double> const &data_value = q->second.value();
	SimpleRange const &data_range = q->second.range();

	SimpleRange target_range = VariableSubsetRange(var);
	data_length = target_range.length();
	this_data = new double[data_length];

	unsigned int i = 0;
	unsigned int nmissing = 0;
	for (RangeIterator p(target_range); !p.atEnd(); p.nextLeft()) {
	    unsigned long j = data_range.leftOffset(p);
	    if (jags_isna(data_value[j])) {
		++nmissing;
	    }
	    this_data[i++] = data_value[j];
	}
	if (nmissing == data_length) {
	    delete [] this_data;
	    this_data = nullptr;
	    data_length = 0;
	}
	/*
	  //OK in JAGS 5.0.0
	else if (nmissing != 0) {
	    delete [] this_data;
	    CompileError(var, var->name() + printRange(target_range),
			 "is partly observed and partly missing");
	}
	*/
    }

    // Check that distribution exists
    string const &distname = distribution->name();
    DistPtr const &dist = distTab().find(distname);
    if (isNULL(dist)) {
	CompileError(distribution, "Unknown distribution:", distname);
    }

    if (!this_data) {
	/* 
	   Special rule for observable functions, which exist both as
	   a Function and a Distribution.  If the node is unobserved,
	   and we find a function matched to the distribution in
	   obsFuncTab, then we create a Logical Node instead.
	*/
	FunctionPtr const &func = obsFuncTab().find(dist);
	if (!isNULL(func)) {
	    LogicalNode *lnode = LogicalFactory::newNode(func, parameters,
							 _model.nchain());
	    _model.addNode(lnode);
            return lnode;
	}
    }	

    /* 
       We allow BUGS-style interval censoring notation for
       compatibility but only allow it if there are no free parameters
       in the distribution
    */
    if (stoch_relation->parameters().size() == 3) {
	ParseTree const *t = stoch_relation->parameters()[2];
	if (t->name() == "I") {
	    for (unsigned int i = 0; i < parameters.size(); ++i) {
		if (!parameters[i]->isFixed()) {
		    CompileError(stoch_relation,
				 "BUGS I(,) notation is only allowed if",
				 "all parameters are fixed");
		}
	    }
	}
    }
    StochasticNode *snode = nullptr;
    if (SCALAR(dist)) {
	snode =  new ScalarStochasticNode(SCALAR(dist), _model.nchain(),
					  parameters, 
					  lBound, uBound);
    }
    else if (VECTOR(dist)) {
	if (lBound != nullptr || uBound != nullptr) {
	    CompileError(stoch_relation, "Distribution cannot be truncated");
	}
	snode = new VectorStochasticNode(VECTOR(dist), _model.nchain(),
					 parameters);
    }
    else if (ARRAY(dist)) {
	if (lBound != nullptr || uBound != nullptr) {
	    CompileError(stoch_relation, "Distribution cannot be truncated");
	}
	snode = new ArrayStochasticNode(ARRAY(dist), _model.nchain(),
					parameters);
    }
    else {
	throw logic_error("Unable to classify distribution");
    }
    _model.addNode(snode);
    
    if (this_data) {
	snode->setData(this_data, data_length);
	delete [] this_data;
    }
    
    return snode;
}

Node * Compiler::allocateLogical(ParseTree const *rel)
{
    ParseTree *expression = rel->parameters()[1];
    Node *node = nullptr;
    vector <Node const *> parents;

    switch (expression->treeClass()) {
    case P_VALUE: 
	node = getConstant(expression->value(), _model.nchain(), false);
	break;
    case P_VAR: case P_FUNCTION: case P_LINK:
	node = getParameter(expression);
	break;
    case P_DISTMOD: case P_DENSITY: case P_COUNTER:
    case P_STOCHREL: case P_DETRMREL: case P_FOR: case P_RELATIONS:
    case P_VECTOR: case P_ARRAY: case P_SUBSET: case P_STRUCT: case P_NULL:
	throw logic_error("Malformed parse tree in Compiler::allocateLogical");
    }

    /*
      Check that there are no values in the data table corresponding to
      this node.
    */
    ParseTree *var = rel->parameters()[0];
    map<string,SArray>::const_iterator q = _data_table.find(var->name());
    if (q != _data_table.end()) {
	vector<double> const &data_value = q->second.value();
	SimpleRange const &data_range = q->second.range();
	SimpleRange target_range = VariableSubsetRange(var);

	for (RangeIterator p(target_range); !p.atEnd(); p.nextLeft()) {
	    unsigned long j = data_range.leftOffset(p);
	    if (!jags_isna(data_value[j])) {
		CompileError(var, var->name() + printRange(target_range),
			     "is a logical node and cannot be observed");
	    }
	}
    }

    return node;
}

void Compiler::allocate(ParseTree const *rel)
{
    /* 
       A relation is uniquely identified by the combination of 
       - address of the ParseTree element that encodes it, and 
       - the current values on counters on the counter stack 
       This pair is used by the set _is_resolved to denote relations
       that already have an allocated node.
    */
    pair<ParseTree const*, vector<unsigned long> >
	relindex(rel, _countertab.counterValues());

    if (_is_resolved.count(relindex) != 0) return;

    ParseTree const * const var = rel->parameters()[0];
    SimpleRange target_range = VariableSubsetRange(var);

    /* 
       Before the node is constructed, We have to ensure that we can
       insert into an array. Deterministic nodes are cached and are
       reusable, but stochastic nodes are not and should only be
       created once for each relation.
    */
    
    if (isNULL(target_range) && !emptyRange(var)) {
	_n_unresolved++; //At least one relation is unresolved
	//There is a range expression but it could not be evaluated
	if (_compiler_mode == PERMISSIVE) {
	    return;
	}
	else if (_compiler_mode == ENFORCING) {
	    CompileError(var, "Unable to calculate subset expression for array",
			 var->name());
	}
	//FIXME: other modes?
    }

    /*
      It is safe to allocate a node
    */
    Node *node = nullptr;
    
    if (rel->treeClass() == P_STOCHREL) {
	node = allocateStochastic(rel);
    }
    else if (rel->treeClass() == P_DETRMREL) {
	node = allocateLogical(rel);
    }
    else {
	throw logic_error("Malformed parse tree in Compiler::allocate");
    }
    
    SymTab &symtab = _model.symtab();
    if (node != nullptr) {
	NodeArray *array = symtab.getVariable(var->name());
	if (!array) {
	    //Undeclared array. Create a new array big enough to
	    //contain the node
	    vector<unsigned long> dim = node->dim();
	    bool lock = false;
	    if (emptyRange(var)) {
		//No range given on LHS. New node fills the whole array
		target_range = SimpleRange(dim);
		lock = true;
	    }
	    else {
		//Range given on LHS. Set initial dimension of array
		//to be equal to the upper limit of the target range.
		dim = target_range.upper();
	    }
	    for (unsigned int i = 0; i < dim.size(); ++i) {
		if (dim[i] == 0) {
		    CompileError(var, "Zero dimension for variable " +
				 var->name());
		}
	    }
	    symtab.addVariable(var->name(), dim);
	    array = symtab.getVariable(var->name()); //FIXME: addVariable should return the array
	    array->insert(node, target_range);
	    if (lock) {
		array->lock();
	    }
	}
	else {
	    // Check if a node is already inserted into this range
	    if (array->getSubset(target_range, _model)) { //FIXME: why is _model an argument here?
		CompileError(var, "Attempt to redefine node",
			     var->name() + printRange(target_range));
	    }
	    array->insert(node, target_range);
	}
	_n_resolved++;
	_is_resolved.insert(relindex);
    }
    else if (_compiler_mode == CLEAN_UNRESOLVED) {
	/* 
	   Remove from the set of unresolved parameters, any array
	   subsets that are defined on the left hand side of a
	   relation
	*/
	_umap.erase(pair<string, Range>(var->name(), target_range));
	
	/*
	  In addition, the parameter might be a *subset* of a node
	  defined on the LHS of a relation.
	*/
	map<pair<string, Range>, set<unsigned long> >::iterator p = _umap.begin();
	while (p != _umap.end()) {
	    pair<string, Range> const &up = p->first;
	    if (up.first == var->name() && target_range.contains(up.second)) {
		_umap.erase(p++);
	    }
	    else {
		p++;
	    }
	}
    }
    else {
	//Failed to resolve relation...
	_n_unresolved++;

	//... but we may still use information on the LHS of the relation

	/* NB Without this code, the following relations cannot be resolved
	      tau[1] ~ dgamma(R,1)
              R[1] <- 1
              sigma[1] <- 1/sqrt(tau)
	*/

	if (!emptyRange(var) && !isNULL(target_range)) {
	    //Range is given on LHS and it can be evaluated
	    NodeArray *array = symtab.getVariable(var->name());
	    if (array) {
		if (array->range().contains(target_range)) {
		    //Check target range is not already occupied
		    if (array->getSubset(target_range, _model)) {
			CompileError(var, "Attempt to redefine node",
				     var->name() + printRange(target_range));
		    }
		}
		else {
		    //Grow array to accommodate target range
		    array->insert(nullptr, target_range);
		}
	    }
	    else {
		//Create new NodeArray big enough to contain target range
		vector<unsigned long> dim = target_range.upper();
		symtab.addVariable(var->name(), dim);
	    }
	}

    }
    
}
    
void Compiler::setConstantMask(ParseTree const *rel)
{
    ParseTree const *var = rel->parameters()[0];
    string const &name = var->name();
    map<string,vector<bool> >::iterator p = _constant_mask.find(name);
    if (p == _constant_mask.end()) {
	return;
    }
    map<string,SArray>::const_iterator q = _data_table.find(name);
    if (q == _data_table.end()) {
	throw logic_error ("Error in Compiler::setConstantMask");
    }
    SimpleRange range = VariableSubsetRange(var);
    SimpleRange const &var_range = q->second.range();
    if (!var_range.contains(range)) {
	CompileError(rel, string("Subset ") + name + print(range) +
		     " inconsistent with data dimensions " +
		     name + print(var_range));
    }
    vector<bool> &mask = p->second;
    for (RangeIterator i(range); !i.atEnd(); i.nextLeft()) {
	mask[var_range.leftOffset(i)] = false;
    }
}

//FIXME: This is obsolete as we allow arrays to grow dynamically
void Compiler::getArrayDim(ParseTree const *p)
{
    /* 
       Called by traverseTree.

       For each node array, calculates the upper bounds of the subset
       expressions on the left-and side of all relations. The results
       are stored in the map _node_array_bounds which is then used to
       calculate the dimensions of the node arrays.
    */

    ParseTree const *var = p->parameters()[0];

    if (var->parameters().empty()) {
	//No index expession => No info on array size
	return;
    }

    SimpleRange new_range = VariableSubsetRange(var);
    vector<unsigned long> const &new_upper = new_range.last();

    string const &name = var->name();
    map<string, vector<unsigned long> >::iterator i = _node_array_bounds.find(name);
    if (i == _node_array_bounds.end()) {
	//Create a new entry
	_node_array_bounds[name] = new_upper;
    }
    else {
	//Check against the existing entry, and modify if necessary
	vector<unsigned long> & ubound = i->second;
	if (new_upper.size() != ubound.size()) {
	    CompileError(var, "Inconsistent dimensions for array", name);
	}
	else {
	    for (unsigned long j = 0; j < ubound.size(); ++j) {
		ubound[j] = max(ubound[j], new_upper[j]);
	    }
	}
    }
}

void Compiler::writeConstantData(ParseTree const *relations)
{
    /* 
       Values supplied in the data table, but which DO NOT
       appear on the left-hand side of a relation, are constants.
       We have to find these values in order to create the 
       constant nodes that form the top level of any graphical
       model.
    */

    //First we set up the constant mask, setting all values to true by
    //default
    map<string, SArray>::const_iterator p;
    for (p = _data_table.begin(); p != _data_table.end(); ++p) {
	pair<string, vector<bool> > apair;
	apair.first = p->first;
	apair.second = vector<bool>(p->second.length(), true);
	_constant_mask.insert(apair);
    }

    //Now traverse the parse tree, setting node array subsets that
    //correspond to the left-hand side of any relation to be false
    traverseBackwards(relations, &Compiler::setConstantMask);

    //Create a temporary copy of the data table containing only
    //data for constant nodes
    map<string, SArray> temp_data_table = _data_table;
    map<string, SArray>::iterator p2;
    for(p2 = temp_data_table.begin(); p2 != temp_data_table.end(); ++p2) {
	string const &name = p2->first;
	SArray &temp_data = p2->second;
	vector<bool> const &mask = _constant_mask.find(name)->second;
	for (unsigned long i = 0; i < temp_data.length(); ++i) {
	    if (!mask[i]) {
		temp_data.setValue(JAGS_NA, i);
	    }
	}
    }

    _model.symtab().writeData(temp_data_table);
}

void Compiler::writeRelations(ParseTree const *relations)
{
    writeConstantData(relations);

    _is_resolved.clear();
    bool lock = false;
    for(;;) {
	_n_resolved = 0;
	_n_unresolved = 0;
	/* 
	   Here we use the abilitiy to sweep forwards and backwards
	   through the relations, allowing rapid compilation of models
	   written in both topological order and reverse topological
	   order. Without this facility, compilation of some large
	   models can be very slow.
	*/
	traverseBackwards(relations, &Compiler::allocate);
	traverseForwards(relations, &Compiler::allocate);
	if (_n_resolved == 0) {
	    if (_n_unresolved == 0) {
		break;
	    }
	    else if (lock) {
		break;
	    }
	    else {
		_model.symtab().lock();
		lock = true;
	    }
	}
    }
    _model.symtab().lock();

    if (_n_resolved == 0 && _n_unresolved > 0) {
	_compiler_mode = ENFORCING; //See getArraySubset
	traverseBackwards(relations, &Compiler::allocate);
	
	/*
	   Some nodes remain unresolved. We need to identify them and
	   print an informative error message for the user.
	   
	   The basic strategy is to identify nodes that appear on the
	   right-hand side of a relation but cannot be resolved.  Then
	   we eliminate the nodes that are defined on the left hand
	   side of a relation.  Whatever nodes are left are used in an
	   expression but never defined (either in a relation or in
	   the data).
	   
	   It may happen that all the unresolved nodes are defined on
	   the left hand side of a relation. This can happen if there
	   is a directed cycle (i.e. the nodes are defined in terms of
	   each other).
	*/

	
	/*
	  Step 2: Collect identifiers for unresolved parameters.
	  These will be held in the map _umap along with the line
	  numbers on which they are used.
	*/
	_compiler_mode = COLLECT_UNRESOLVED; //See getArraySubset
	traverseBackwards(relations, &Compiler::allocate);
	if (_umap.empty()) {
	    //Not clear what went wrong here, so throw a generic error message
	    throw runtime_error("Unable to resolve relations");
	}
	//Take a back-up copy of unresolved parameters
	map<pair<string,Range>, set<unsigned long> > umap_copy = _umap;

	/*
	  Step 3: Eliminate parameters that appear on the left of a relation
	*/
	_compiler_mode = CLEAN_UNRESOLVED;
	traverseBackwards(relations, &Compiler::allocate);

	/* 
	   Step 4: Informative error message for the user
	*/
	ostringstream oss;
	if (!_umap.empty()) {
	    //Undefined parameter message
	    oss << "Unable to resolve the following parameters:\n";
	    for (map<pair<string, Range>, set<unsigned long> >::iterator p =
		     _umap.begin(); p != _umap.end(); ++p)
	    {
		oss << p->first.first << printRange(p->first.second);
		oss << " (line ";
		set<unsigned long> const &lines = p->second;
		for (set<unsigned long>::const_iterator i = lines.begin();
		     i != lines.end(); ++i)
		{
		    if (i != lines.begin()) { oss << ", "; }
		    oss << *i;
		}
		oss << ")\n";
	    }
	    oss << "Either supply values for these nodes with the data\n"
		<< "or define them on the left hand side of a relation.";
	}
	else {
	    //Directed cycle message
	    oss << "Possible directed cycle involving some or all\n"
		<< "of the following nodes:\n";
	    for (map<pair<string, Range>, set<unsigned long> >::const_iterator p =
		     umap_copy.begin(); p != umap_copy.end(); ++p)
	    {
		oss << p->first.first << printRange(p->first.second) << "\n";
	    }
	}
	throw runtime_error(oss.str());
    }

    _is_resolved.clear();
}

void Compiler::traverseBackwards(ParseTree const *relations, CompilerMemFn FUN)
{
    /* 
       Traverse parse tree, expanding FOR loops and applying function
       fun to relations.

       The code is interpreted from back to front, which is most efficient
       when the relations are written in reverse topological order.
    */

    vector<ParseTree*> const &relation_list = relations->parameters();
    for (auto p = relation_list.rbegin(); p != relation_list.rend(); ++p) 
    {
	TreeClass tc = (*p)->treeClass();
	if (tc == P_STOCHREL || tc == P_DETRMREL) {
	    (this->*FUN)(*p);
	}
	else if (tc == P_FOR) {
	    //Expand for loops
	    ParseTree *var = (*p)->parameters()[0];
	    vector<unsigned long> counter_range = CounterRange(var);
	    if (!counter_range.empty()) {
		Counter *counter = _countertab.pushCounter(var->name(),
							   counter_range);
		for (; !counter->atEnd(); counter->next()) {
		    traverseBackwards((*p)->parameters()[1], FUN);
		}
		_countertab.popCounter();
	    }
	}
    }
}

void Compiler::traverseForwards(ParseTree const *relations, CompilerMemFn FUN)
{
    /* 
       Traverse parse tree, expanding FOR loops and applying function
       fun to relations.

       The code is interpreted from top to bottom, which is most
       efficient when the relations are written in topological order.
    */

    vector<ParseTree*> const &relation_list = relations->parameters();
    for (auto p = relation_list.begin(); p != relation_list.end(); ++p)
    {
	TreeClass tc = (*p)->treeClass();
	if (tc == P_STOCHREL || tc == P_DETRMREL) {
	    (this->*FUN)(*p);
	}
	else if (tc == P_FOR) {
	    //Expand for loops
	    ParseTree *var = (*p)->parameters()[0];
	    vector<unsigned long> counter_range = CounterRange(var);
	    if (!counter_range.empty()) {
		Counter *counter = _countertab.pushCounter(var->name(),
							   counter_range);
		for (; !counter->atEnd(); counter->next()) {
		    traverseForwards((*p)->parameters()[1], FUN);
		}
		_countertab.popCounter();
	    }
	}
    }
}

Compiler::Compiler(BUGSModel &model, map<string, SArray> const &data_table)
    : _model(model), _countertab(), 
      _data_table(data_table), _n_resolved(0), _n_unresolved(0), 
      _is_resolved(), _compiler_mode(PERMISSIVE),
      _index_expression(0), _index_nodes()
{
    if (_model.nodes().size() != 0)
	throw invalid_argument("Non empty graph in Compiler constructor");
    if (_model.symtab().size() != 0)
	throw invalid_argument("Non empty symtab in Compiler constructor");
}

void Compiler::declareVariables(vector<ParseTree*> const &dec_list)
{
  for (auto p = dec_list.begin() ; p != dec_list.end(); ++p) {
    if ((*p)->treeClass() != P_VAR) {
      throw invalid_argument("Expected variable expression");
    }
  }

  //checkDecNames(dec_list, _data_table);

  for (auto p = dec_list.begin() ; p != dec_list.end(); ++p) {
    ParseTree const *node_dec = *p;
    string const &name = node_dec->name();
    unsigned long ndim = node_dec->parameters().size();
    if (ndim == 0) {
	// Variable is scalar
	_model.symtab().addVariable(name, vector<unsigned long>(1,1));
    }
    else {
	// Variable is an array
	vector<unsigned long> dim(ndim);
	for (unsigned int i = 0; i < ndim; ++i) {
	    vector<unsigned long> dim_i;
	    if (node_dec->parameters()[i]->treeClass() == P_NULL) {
		CompileError(node_dec, "Missing dimension in declaration of", name);
	    }
	    if (!indexExpression(node_dec->parameters()[i], dim_i)) {
		CompileError(node_dec, "Unable to calculate dimensions of",
			     name);
	    }
	    if (dim_i.empty()) {
		CompileError(node_dec, "NULL dimension in declaration of", 
			     name);
	    }
	    if (dim_i.size() != 1) {
		CompileError(node_dec, 
			     "Vector-valued dimension in declaration of", name);
	    }
	    if (dim_i[0] <= 0) {
		CompileError(node_dec, "Non-positive dimension for node", name);
	    }
	    dim[i] = static_cast<unsigned int>(dim_i[0]);
	}
	_model.symtab().addVariable(name, dim);
    }

    //Lock declared arrays
    NodeArray *array = _model.symtab().getVariable(name);
    array->lock();

    //Check consistency with data, if supplied
    map<string, SArray>::const_iterator q = _data_table.find(name);
    if (q != _data_table.end()) {
	if (q->second.range() != array->range()) {
	    string msg = string("Dimensions of ") + name + 
		" in declaration (" + printRange(array->range()) + 
		") conflict with dimensions in data (" + 
		printRange(q->second.range()) + ")";
	    CompileError(node_dec, msg);
	}
    }

  }
}

void Compiler::undeclaredVariables(ParseTree const *prelations)
{
    // Get undeclared variables from data table
    for (map<string, SArray>::const_iterator p = _data_table.begin();
	 p != _data_table.end(); ++p) 
    {
	string const &name = p->first;
	NodeArray *array = _model.symtab().getVariable(name);
	if (!array) {
	    _model.symtab().addVariable(name, p->second.dim(false));
	    array = _model.symtab().getVariable(name);
	    array->lock();
	}
    }

    // Get names of variables on the LHS of a relation
    getLHSVars(prelations);
}

/* 
   We use construct-on-first-use for the lookup tables used by the
   compiler.  By dynamically allocating a table, we ensure that its
   destructor is never called - the memory is simply returned to the
   OS on exit.

   This fixes a nasty exit bug.  We cannot guarantee the order that
   static destructors are called in.  Therefore, a segfault can occur
   if a module tries to unload itself from a table that has already
   been destroyed.

   See also Model.cc, where the same technique is used for factory
   lists.
*/

DistTab &Compiler::distTab()
{
    static DistTab *_disttab = new DistTab();
    return *_disttab;
}

FuncTab &Compiler::funcTab()
{
    static FuncTab *_functab = new FuncTab();
    return *_functab;
}

ObsFuncTab &Compiler::obsFuncTab()
{
    static ObsFuncTab *_oftab = new ObsFuncTab();
    return *_oftab;
}

/*
MixtureFactory& Compiler::mixtureFactory1()
{
   return _mixfactory1;
}

MixtureFactory& Compiler::mixtureFactory2()
{
   return _mixfactory2;
}
*/

BUGSModel &Compiler::model() const
{
    return _model;
}

} //namespace jags
