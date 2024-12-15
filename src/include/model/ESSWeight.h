#ifndef ESS_WEIGHT_H_
#define ESS_WEIGHT_H_

#include <model/WeightMonitor.h>

namespace jags {

    class MonitorStat;
    
    /**
     * @short Stores running mean of a given stat
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
