#ifndef STOCHASTIC_INDEX_H_
#define STOCHASTIC_INDEX_H_

#include <vector>
#include <string>

namespace jags {

    class SymTab;
    class Node;
    
    class StochasticIndex {
    private:
	static const std::vector<unsigned long> _null_fixed;
	Node const *_variable;
	std::vector<unsigned long> const &_fixed;
    public:
	StochasticIndex(Node const *node);
	StochasticIndex(std::vector<unsigned long> const &indices);
	Node const *variableIndex() const;
	std::vector<unsigned long> const &fixedIndex() const;
	std::string deparse(SymTab const &symtab) const;
	bool isVariable() const;
	static bool isValid(Node const *node);
    };

    bool lt(StochasticIndex const &lhs, StochasticIndex const &rhs);
    bool lt(std::vector<StochasticIndex> const &lhs,
	    std::vector<StochasticIndex> const &rhs);

    struct less_stoch_indices {
	bool operator()(std::vector<StochasticIndex> const &lhs,
			std::vector<StochasticIndex> const &rhs) const {
	    return lt(lhs, rhs);
	}
    };

    std::string printValue(std::vector<StochasticIndex> const &indices,
			   unsigned long chain);
    
}
    
#endif /* STOCHASTIC_INDEX_H_ */
    
