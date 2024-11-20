#ifndef PD_MONITOR_H_
#define PD_MONITOR_H_

#include <model/Monitor.h>

#include <vector>

namespace jags {

    class StochasticNode;
    struct RNG;
    
namespace dic {

    class PDMonitor : public Monitor {
	std::vector<StochasticNode const *> _snodes;
	std::vector<RNG *> _rngs;
	unsigned int _nrep;
	std::vector<double> _values;
	std::vector<double> _weights;
	double _scale;
	unsigned long _nchain;
    public:
	PDMonitor(std::vector<StochasticNode const *> const &snodes,
		  std::vector<RNG *> const &rngs,
		  unsigned int nrep, double scale=1);
	~PDMonitor() override;
	std::vector<unsigned long> dim() const override;
	void value(std::vector<double> &v, unsigned int chain) const override;
	bool poolChains() const override;
	bool poolIterations() const override;
	void update(unsigned int chain) override;
	virtual double weight(StochasticNode const *snode,
			      unsigned int ch) const;
    };

}}

#endif /* PD_MONITOR_H_ */
