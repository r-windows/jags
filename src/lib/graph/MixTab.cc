#include <graph/MixTab.h>
#include <graph/Node.h>

#include <stdexcept>

using std::vector;
using std::map;
using std::logic_error;

namespace jags {

    static SimpleRange
    mkRange(map<vector<unsigned long>, Node const *> const &mixmap)
    {
	/* 
	   Calculates the smallest SimpleRange enclosing the index
	   values in the MixMap.  Also checks for consistency in the
	   lengths of the index values.
	*/
	auto p = mixmap.begin();

	unsigned long N = p->first.size();
	vector<unsigned long> lower(p->first), upper(p->first);
	
	for (++p ; p != mixmap.end(); ++p) {
	    if (p->first.size() != N) {
		throw logic_error("index size mismatch in MixTab");
	    }
	    for (unsigned long j = 0; j < N; ++j) {
		unsigned long i = p->first[j];
		if (i < lower[j]) lower[j] = i;
		if (i > upper[j]) upper[j] = i;
	    }
	}
	
	return SimpleRange(lower, upper);
    }

    static vector<unsigned long> const&
    mkDim(map<vector<unsigned long>, Node const *> const &mixmap)
    {
	auto p = mixmap.begin();
	
	vector<unsigned long> const &dim0 = p->second->dim();
	
	for (++p ; p != mixmap.end(); ++p) {
	    if (p->second->dim() != dim0) {
		throw logic_error("parent dimension mismatch in MixTab");
	    }
	}
	
	return dim0;
    }
    
    MixTab::MixTab(map<vector<unsigned long>, Node const *> const &mixmap)
	: _range(mkRange(mixmap)), _dim(mkDim(mixmap)), _nodes(_range.length(), nullptr)
    {
	for (auto p = mixmap.begin(); p != mixmap.end(); ++p)
	{
	    _nodes[_range.leftOffset(p->first)] = p->second;
	}
    }
    
    Node const * MixTab::getNode(vector<unsigned long> const &index) const
    {
	if (!_range.contains(index)) return nullptr;

	unsigned long offset = _range.leftOffset(index);
	return _nodes[offset];
    }

    SimpleRange const &MixTab::range() const
    {
	return _range;
    }

    vector<unsigned long> const &MixTab::dim() const
    {
	return _dim;
    }
    
    vector<Node const *> const &MixTab::nodes() const
    {
	return _nodes;
    }

}

