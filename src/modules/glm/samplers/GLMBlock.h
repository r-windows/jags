#ifndef GLM_BLOCK_H_
#define GLM_BLOCK_H_

#include "GLMMethod.h"

#include <vector>

namespace jags {

    struct RNG;
    class GraphView;
    class SingletonGraphView;

namespace glm {

    class Outcome;

    /**
     * @short Block sampler for generalized linear models.
     */
    class GLMBlock : public GLMMethod {
	bool update0(RNG*);
    public:
	/**
	 * Constructor.
	 *
	 * @param view Pointer to a GraphView object for all sampled nodes.
	 *
	 * @param sub_views Vector of pointers to SingletonGraphView
	 * objects with length equal to the number of sampled
	 * nodes. Each sub-view corresponds to a single sampled node.
	 *
	 * @param outcomes Vector of pointers to Outcome objects with length
	 * equal to the number of stochastic children of the sampled nodes.
	 * The GLMMethod objects takes ownership of each Outcome in the vector
	 * and frees the memory in the destructor.
	 * 
	 * @param chain Number of the chain (starting from 0) to which
	 * the sampling method will be applied.
	 */
	GLMBlock(GraphView const *view, 
		 std::vector<SingletonGraphView const *> const &sub_views,
		 std::vector<Outcome *> const &outcomes,
		 unsigned int chain, cholmod_common *wk);
	/**
	 * Updates the regression parameters by treating the GLM as a
	 * linear model (LM).  All regression parameters are updated
	 * together in a block.
	 *
	 * @param rng Random number generator used for sampling
	 */
	void update(RNG *rng) override;
	/**
	 * Updates auxiliary variables (if any) marginalizing over the
	 * value of the linear predictor.  The default method does
	 * nothing. This is currently only used by the HolmesHeld
	 * subclass and is documented in the code for that class.
	 */
	virtual void updateAuxiliary(cholmod_dense *w, RNG *rng);
    };

}}

#endif /* GLM_BLOCK_H_ */
