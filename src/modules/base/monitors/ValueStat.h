#ifndef VALUE_STAT_H_
#define VALUE_STAT_H_

#include <model/NodeArraySubset.h>
#include <model/MonitorStat.h>

#include <vector>

namespace jags {
    namespace base {

	/**
	 * @short 
	 */
	class ValueStat : public MonitorStat {
	    NodeArraySubset _subset;
	public:
	    ValueStat(NodeArraySubset const &subset);
	    std::vector<unsigned long> dim() const override;
	    std::vector<double> value(unsigned int chain) const override;
	    unsigned long length() const override;
	};
	
    }
}

#endif /* VALUE_STAT_H_ */
