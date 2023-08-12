#ifndef MIXTURE_NODE_H_
#define MIXTURE_NODE_H_

#include <graph/DeterministicNode.h>
#include <model/StochasticIndex.h>
#include <map>
#include <vector>

namespace jags {

    class Node;
    class MixTab;

/**
 * @short Node for mixture models.
 *
 * A mixture node copies its value from one of several parents, based
 * on the current value of a vector of index nodes.
 *
 * In the BUGS language, mixture nodes are represented using nested
 * indexing. For example, in the deterministic relation
 *
 * <pre>y[i] <- x[ind[i]]</pre>
 *
 * y[i] is a mixture node if ind[i] is unobserved.  If the possible
 * values of ind[i] are 1...M, then the parents of y[i] are ind[i],
 * x[1], ... x[M].
 */
class MixtureNode : public DeterministicNode {
    std::vector<StochasticIndex> const _indices;
    MixTab const &_table;
    unsigned long _nindex;
    bool _discrete;
    std::vector<Node const *> _active_parents;
    void updateActive(unsigned int chain);
public:
    /**
     * Constructs a MixtureNode. 
     *
     * @param index Vector of index nodes. These must be
     *  discrete-valued, scalar, and unobserved.
     *
     * @param nchain Number of chains
     *
     * @param mixtab a MixTab object which associates each possible
     * value of the index nodes with a single parent.
     */
    MixtureNode(std::vector<StochasticIndex> const &index,
		unsigned int nchain, MixTab const &mixtab);
    /**
     * Copies the value of the active parent
     */
    void deterministicSample(unsigned int chain) override;
    /**
     * Returns a pointer to the currently active parent (i.e. the one
     * determined by the current index values) in the given chain.
     */
    Node const *activeParent(unsigned int chain) const;
    /**
     * Returns the number of index nodes.
     */
    unsigned long index_size() const;
    /**
     * Returns a pointer to the MixTab
     */
    MixTab const *mixTab() const;
    /**
     * A MixtureNode is discrete valued if all of its possible
     * parents are discrete
     */
    bool isDiscreteValued() const override;
    /**
     * A MixtureNode preserves all closed classes if none of its index
     * nodes are descendants of X. It is never fixed.
     */
    bool isClosed(std::set<Node const *> const &ancestors, 
		  ClosedFuncClass fc, bool fixed) const override;
    /**
     * This function always returns true. It ignores possible errors
     * in the index nodes.  This is because the deterministicSample
     * member function already checks that the index value is valid
     * and will throw an runtime error if it is not.  Repeating these
     * calculations in the checkParentValues function would therefore
     * be redundant.
     */
    bool checkParentValues(unsigned int chain) const override;
    /**
     * Creates a name for the mixture node of the form.
     *
     * <pre>mixture(index=[p,q], parents=X[1,1,4]...X[2,3,4])</pre>
     *
     * Only the first and last parent nodes are listed to save space.
     *
     * Exceptionally, the name of a mixture node is not a reconstruction
     * of its BUGS-language definition.
     */
    std::string deparse(std::vector<std::string> const &parents) const override;
    //DeterministicNode *clone(std::vector<Node const *> const &parents) const;
    bool hasGradient(Node const *arg) const override;
    void gradient(double *grad, Node const *arg, unsigned int chain)
	const override;    
};

bool isMixture(Node const *);
MixtureNode const * asMixture(Node const *);

} /* namespace jags */

#endif /* MIXTURE_NODE_H_ */
