#ifndef MIX_TAB_H_
#define MIX_TAB_H_

#include <sarray/SimpleRange.h>

#include <vector>
#include <map>

namespace jags {

    class Node;

    /**
     * @short Helper class for MixtureNode
     *
     * Evaluation of a MixtureNode uses a MixTab, which takes the
     * current map value (a vector of integers) and returns a
     * pointer to the parent node from which the MixtureNode takes its
     * current value.
     */
    class MixTab {
	const SimpleRange _range;
	std::vector<unsigned long> const &_dim;
	std::vector<Node const *> _nodes;
      public:
	/**
	 * Constructs a MixTab from a MixMap
	 */
	MixTab(std::map<std::vector<unsigned long>, Node const *> const &mixmap);
	/** 
	 * Returns a pointer to the node corresponding to the given
	 * index.  If there is no node matching the index, a NULL
	 * pointer is returned.
	 */
	Node const * getNode(std::vector<unsigned long> const &index) const;
	/**
	 * Returns a minimal range that contains all valid indices.
	 * Required by the LDA sampler in the mix module.
	 */
	SimpleRange const &range() const;
	/**
	 * Returns the vector of possible values of getNode.
	 */
	std::vector<Node const *> const &nodes() const;
	/**
	 * Returns the common dimension of all nodes in the MixTab
	 */
	std::vector<unsigned long> const &dim() const;
    };

} /* namespace jags */

#endif /* MIX_TAB_H_ */
