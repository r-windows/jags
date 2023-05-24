#ifndef MIXTURE_NODE_ERROR_H_
#define MIXTURE_NODE_ERROR_H_

#include <graph/NodeError.h>
#include <iostream>

namespace jags {

    class MixtureNode;
    class SymTab;

    /**
     * @short Exception class for Mixture Nodes
     */
    class MixtureNodeError : public NodeError {
	MixtureNode const * _mixnode;
	unsigned long _chain;
    public:
	MixtureNodeError(MixtureNode const *mixnode, std::string const &msg,
			 unsigned long chain);
	virtual void
	printMessage(std::ostream &out, SymTab const &symtab) const override;
    };

} /* namespace jags */

#endif /* MIXTURE_NODE_ERROR_H_ */
