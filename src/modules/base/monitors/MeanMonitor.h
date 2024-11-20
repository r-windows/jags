#ifndef VALUE_MEAN_MONITOR_H_
#define VALUE_MEAN_MONITOR_H_

#include <model/MeanMonitor.h>
#include <model/NodeArraySubset.h>

#include <vector>

namespace jags {
namespace base {

    /**
     * @short Stores running mean of a given Node
     */
    class ValueMeanMonitor : public MeanMonitor {
	NodeArraySubset _subset;
    public:
	ValueMeanMonitor(NodeArraySubset const &subset);
	std::vector<unsigned long> dim() const override;
	std::vector<double> stat(unsigned int chain) override;
    };

}}

#endif /* VALUE_MEAN_MONITOR_H_ */
