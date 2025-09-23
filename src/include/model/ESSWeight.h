#ifndef ESS_WEIGHT_H_
#define ESS_WEIGHT_H_

#include <model/WeightMonitor.h>

namespace jags {

    class MonitorStat;
    
    /**
     * @short Effective sample size of the weights from a weighted
     * stat
     *
     * The effective sample size (ESS) of a weighted sample is the
     * number of unweighted samples that would contain the same
     * information. It is a function of the coefficient of variation
     * of the weights.
     *
     * ESS does not take into account serial correlation of either the
     * weights or the monitored stat and therefore gives an upper
     * bound on the effective sample size of samples from a Markov
     * chain.
     */
    class ESSWeight : public WeightMonitor {
	std::vector<std::vector<double>> _W, _W2;
    public:
	ESSWeight(std::vector<Node const *> const &nodes, MonitorStat *stat);
	void update(unsigned int chain) override;
	std::vector<double> value(unsigned int chain) const override;
	bool poolIterations() const override;
    };

}

#endif /* ESS_WEIGHT_H_ */
