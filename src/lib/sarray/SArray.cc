#include <config.h>
#include <sarray/SArray.h>
#include <util/nainf.h>

#include <stdexcept>
#include <algorithm>

using std::vector;
using std::logic_error;
using std::length_error;
using std::copy;
using std::string;

namespace jags {

    char const * asChar(DimTag tag)
    {
	switch(tag) {
	case DIM_VALUE:
	    return "value";
	case DIM_CHAIN:
	    return "chain";
	case DIM_ITER:
	    return "iteration";
	}
	return ""; //-Wall
    }
	
SArray::SArray(vector<unsigned long> const &dim)
    : _range(dim), _value(_range.length(), JAGS_NA), _discrete(false),
      _dimnames(dim.size())
{
}

SArray::SArray(SArray const &orig)
    : _range(orig._range), _value(orig._value), _discrete(orig._discrete),
      _dimnames(orig._dimnames), _dimtags(orig._dimtags), _value_names(orig._value_names)
{
}

SimpleRange const &SArray::range() const
{
    return _range;
}

void SArray::setValue(vector<double> const &x)
{
    if (x.size() != _value.size()) {
	throw length_error("Length mismatch error in SArray::setValue");
    }
    else {
        copy(x.begin(), x.end(), _value.begin());
	_discrete = false;
    }
}

void SArray::setValue(double value, unsigned long i)
{
    if (i >= _range.length()) {
	throw logic_error("Attempt to set value of invalid element of SArray");
    }
    else {
	_value[i] = value;
    }
}

vector<double> const &SArray::value() const
{
    return _value;
}

bool SArray::isDiscreteValued() const
{
    return _discrete;
}

vector<DimTag> const &SArray::dimTags() const
{
    return _dimtags;
}

void SArray::setDimTags(vector<DimTag> const &tags)
{
    if (tags.empty() || tags.size() == _range.ndim(false)) {
	_dimtags = tags;
    }
    else {
	throw length_error("Invalid length in SArray::setDimTags");
    }
}

vector<string> const &SArray::dimNames(unsigned long i) const
{
    if (i >= _range.ndim(false))
	throw logic_error("Dimension out of range in SArray::dimNames");

    return _dimnames[i];
}

void SArray::setDimNames(vector<string> const &names, unsigned long i)
{
    if (i >= _range.ndim(false))
	throw logic_error("Dimension out of range in SArray::setSDimNames");

    if (names.empty() || names.size() == _range.dim(false)[i]) {
	_dimnames[i] = names;
    }
    else {
	throw length_error("Invalid length in SArray::setSDimNames");
    }
}

    vector<string> const &SArray::valueNames() const
    {
	return _value_names;
    }

    void SArray::setValueNames(vector<string> const &names)
    {
	unsigned long vlen=0;
	for (unsigned long i = 0; i < _dimtags.size(); ++i) {
	    if (_dimtags[i] == DIM_VALUE) {
		if (vlen == 0) {
		    vlen = 1;
		}
		vlen *= _range.dim(false)[i];
	    }
	}
	if (names.empty() || names.size() == vlen) {
	    _value_names = names;
	}
	else {
	    throw length_error("Invalid length in SArray::setValueNames");
	}
    }
    
} //namespace jags
