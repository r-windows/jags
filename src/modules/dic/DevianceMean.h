#ifndef DEVIANCE_MEAN_H_
#define DEVIANCE_MEAN_H_

#include <model/Monitor.h>

namespace jags {

class StochasticNode;

namespace dic {

    class DevianceMean : public Monitor {
	std::vector<StochasticNode const *> _snodes;
	std::vector<double>  _mdev; 
    public:
	DevianceMean(std::vector<StochasticNode const *> const &nodes);
	std::vector<unsigned long> dim() const override;
	void value(std::vector<double> &v, unsigned int chain) const override;
	void update(unsigned int chain) override;
	bool poolChains() const override;
	bool poolIterations() const override;
    };

}}

#endif /* DEVIANCE_MEAN_H_ */
