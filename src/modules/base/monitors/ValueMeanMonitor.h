#ifndef VALUE_MEAN_MONITOR_H_
#define VALUE_MEAN_MONITOR_H_

#include <model/MeanMonitor.h>

namespace jags {

    class NodeArraySubset;
    
    namespace base {

	/**
	 * @short Stores running mean of a given Node
	 */
	class ValueMeanMonitor : public MeanMonitor {
	public:
	    ValueMeanMonitor(NodeArraySubset const &subset);
	};

    }
}

#endif /* VALUE_MEAN_MONITOR_H_ */
