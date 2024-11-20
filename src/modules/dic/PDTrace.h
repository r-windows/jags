#ifndef PD_TRACE_H_
#define PD_TRACE_H_

#include <model/Monitor.h>

#include <vector>

namespace jags {

class StochasticNode;
struct RNG;

namespace dic {

    class PDTrace : public Monitor {
	std::vector<StochasticNode const *> _snodes;
	std::vector<RNG *> _rngs;
	unsigned int _nrep;
	unsigned long _nchain;
	std::vector<double> _values;

    public:
	PDTrace(std::vector<StochasticNode const *> const &snodes,
		std::vector<RNG*> const &rngs, unsigned int nrep);
	~PDTrace() override;
	std::vector<unsigned long> dim() const override;
	void value(std::vector<double> &v, unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
	void update(unsigned int chain) override;
    };

}}

#endif /* PD_TRACE_H_ */
