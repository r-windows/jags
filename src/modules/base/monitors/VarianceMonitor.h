#ifndef VALUE_VAR_MONITOR_H_
#define VALUE_VAR_MONITOR_H_

#include <model/VarMonitor.h>
#include <model/NodeArraySubset.h>

#include <vector>

namespace jags {
    namespace base {

	/**
	 * @short Stores running variance of a given Node
	 */
	class ValueVarMonitor : public VarMonitor {
	    NodeArraySubset _subset;
	public:
	    ValueVarMonitor(NodeArraySubset const &subset);
	    std::vector<unsigned long> dim() const override;
	    std::vector<double> stat(unsigned int chain) override;
	};
	
    }
}

#endif /* VALUE_VAR_MONITOR_H_ */
