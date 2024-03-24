#include <model/NodeArray.h>
#include "ReadData.h"
#include <sarray/SArray.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using std::cout;
using std::cerr;
using std::endl;

using std::map;
using std::string;
using std::vector;
using std::max;
using std::min;
using std::multiplies;
using std::accumulate;

using jags::SArray;
using jags::ParseTree;
using jags::P_VAR;
using jags::P_VECTOR;
using jags::P_ARRAY;
using jags::P_FUNCTION;
using jags::P_VALUE;
using jags::P_STRUCT;

static bool checkStruct(ParseTree const *p)
{
    return p->treeClass() == P_STRUCT;
}

static bool checkVector(ParseTree const *p)
{
    return p->treeClass() == P_VECTOR;
}

static bool checkSequence(ParseTree const *p)
{
    return p->treeClass() == P_FUNCTION && p->name() == ":";
}

static void readVector(ParseTree const *p, vector<double> &val)
{
    // Collection vector c(a,b,...)
    unsigned long length = p->parameters().size();
    val.resize(length);
    for (unsigned long i = 0; i < length; ++i) {
	val[i] = p->parameters()[i]->value();
    }
}

static void readSequence(ParseTree const *p, vector<double> &val)
{
    // Vector as sequence a:b
    double start = p->parameters()[0]->value();
    double end = p->parameters()[1]->value();
    double lower = min(start, end);
    double upper = max(start, end);
    unsigned long length = static_cast<unsigned long>(upper - lower + 1);
    val.resize(length);
    double direction = (start <= end) ? 1 : -1;
    for (unsigned long i = 0; i < length; ++i) {
	val[i] = start + direction * i;
    }
}

static bool readDimVector(ParseTree const *p, string const &name, vector<unsigned long> &dim)
{
    // Dimension as vector c(a,b,...)
    unsigned long length = p->parameters().size();
    dim.resize(length);
    for (unsigned long i = 0; i < length; ++i) {
	double dim_i = p->parameters()[i]->value();
	if (dim_i < 0) {
	    cerr << "Negative dimension for variable " << name << endl; 
	    return false;
	}
	dim[i] = static_cast<unsigned long>(dim_i);
	if (dim[i] == 0UL) {
	    cerr << "Zero dimension for variable " << name << endl; 
	    return false;
	}
    }
    return true;
}

static bool readDimSequence(ParseTree const *p, string const &name, vector<unsigned long> &dim)
{
    // Dimension as integer sequence a:b
    double start = p->parameters()[0]->value();
    double end = p->parameters()[1]->value();
    double lower = min(start, end);
    double upper = max(start, end);
    if (lower <= 0) {
	cerr << "Invalid sequence " << name << " = " << start << ":" << end << endl;
	return false;
    }
    unsigned long length = static_cast<unsigned long>(upper - lower + 1);
    dim.resize(length);
    double direction = (start <= end) ? 1 : -1;
    for (unsigned long i = 0; i < length; ++i) {
	dim[i] = static_cast<unsigned long>(start + direction * i);
    }
    return true;
}

static bool readDim(ParseTree const *p, string const &name,
		    vector<unsigned long> &dim)
{
    if (checkVector(p)) {
	return readDimVector(p, name, dim);
    }
    else if (checkSequence(p)) {
	return readDimSequence(p, name, dim);
    }
    else {
	cerr << "Invalid dimension attribute for variable " << name << endl;
	return false;
    }
}


bool readRData(vector<ParseTree*> const *array_list, 
	       map<string, SArray> &table,
	       string &rngname)
{
    for (auto p = array_list->begin(); p != array_list->end(); ++p) {

	if ((*p)->treeClass() != P_ARRAY || (*p)->parameters().empty()) {
	    cerr << "Error reading R dump data.";
	    return false;
	}
	ParseTree const *rhs = (*p)->parameters()[0];
	string const &name = (*p)->name();
	
	if (rhs->treeClass() == P_VAR) {
	    /*
	      Assignments of the form "foo" <- "bar" The only type
	      currently allowed is ".RNG.name" <- "bar"
	    */
	    if (name != ".RNG.name") {
		cerr << "Unrecognized string assignment. "
		     << "Expecting \".RNG.name\"" << endl;
		return false;
	    }
	    rngname = rhs->name();
	}
	else {
	    /* Check to see if name is already in table */
	    if (table.find(name) != table.end()) {
		cerr << "WARNING: Replacing " << name << endl;
		table.erase(table.find(name));
	    }
	
	    vector<double> value;
	    vector<unsigned long> dim;

	    if (checkVector(rhs)) {
		readVector(rhs, value);
	    }
	    else if (checkStruct(rhs)) {

		readVector(rhs->parameters()[0], value);
	    
		if (rhs->parameters().size() == 2) {
		    // Array has dimension attribute
		    ParseTree const *pdim = rhs->parameters()[1];
		    if(!readDim(pdim, name, dim)) return false;		
		    // Check that dimension is consistent with length
		    if (accumulate(dim.begin(), dim.end(), 1UL, multiplies<unsigned long>()) != value.size()) {
			cerr << "Dimension for variable " << name <<
			    " inconsistent with length" << endl;
			return false;
		    }
		}
	    }
	    else if (checkSequence(rhs)) {
		readSequence(rhs, value);
	    }
	    else {
		cerr << "Error reading R dump data" << endl;
		return false;
	    }


	    /* Now assign it to an SArray */
	    if (dim.empty()) {
		dim.push_back(value.size());
	    }
	    SArray sarray(dim);
	    sarray.setValue(value);
    
	    /* Since there is no default constructor for SArray, we can't
	       use the shorthand table[names[i]] = par;
	    */
	    table.insert(map<string, SArray>::value_type(name, sarray));

#ifdef DEBUG_JAGS
	    std::cout << "Reading " << name << "[";
	    for(unsigned int j = 0; j < dim.size(); j++) {
		if (j > 0) {
		    std::cout << ",";
		}
		std::cout << dim[j];
	    }
	    std::cout << "]" << std::endl;
#endif
	    
	}
    }
    return true;
}
