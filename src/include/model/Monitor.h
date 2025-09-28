#ifndef MONITOR_H_
#define MONITOR_H_

#include <sarray/SArray.h>
#include <sarray/Range.h>

#include <vector>
#include <string>

namespace jags {

    class MonitorStat;
    class Node;
    
/**
 * @short Analyze sampled values 
 *
 * This is an abstract class for objects that analyze and/or store sampled
 * statistics from a given set of nodes.
 */
class Monitor {
    std::vector<Node const *> _nodes;
    unsigned int _nchain;
    unsigned int _niter;
protected:
    MonitorStat *_stat;
public:
    Monitor(std::vector<Node const *> const &nodes, MonitorStat *stat);
    virtual ~Monitor();
    /**
     * Updates the monitor. 
     *
     * This will call the virtual function update for each chain and increment
     * the iteration count by 1.
     */
    void update();
    /*
     * Updating should be an ammortized constant time operation.
     * Failure to guarantee this may cause long MCMC runs to slow down
     * dramatically.  This is particularly important if the monitor
     * needs to allocate new memory for stored samples.
     */
    virtual void update(unsigned int chain) = 0;
    /**
     * Returns the number of iterations recorded by the Monitor, i.e. the
     * number of times that Monitor#update has been called.
     */
    unsigned long niter() const;
    /**
     * Number of chains of the monitored nodes
     */
    unsigned int nchain() const;
    /**
     * Returns the vector of nodes from which the monitor's value is
     * derived.
     */
    std::vector<Node const *> const &nodes() const;
    /**
     * Returns true if the monitor has a single value for multiple chains
     */
    virtual bool poolChains() const = 0;
    /**
     * Returns true if the monitor has a single value for multiple iterations
     */
    virtual bool poolIterations() const = 0;
    /**
     * Returns the vector of monitored values for the given chain.
     */
    virtual std::vector<double> value(unsigned int chain) const = 0;
    /**
     * Returns the length of the unit monitored value corresponding to
     * a single iteration in a single chain. The default
     * implementation returns the length of the stat.
     */
    virtual unsigned long length() const;
    /**
     * Returns the dimensions of the unit monitored value
     * corresponding to a single iteration in a single chain. The
     * default implementation returns the dimension of the stat.
     */
    virtual std::vector<unsigned long> dim() const;
    /**
     * Dumps the monitored values to an SArray. 
     *
     * The SArray will have informative dimnames. In particular, the
     * dimnames "iteration" and "chain" are used if there are
     * distinct values for each iteration and each chain,
     * respectively.
     *
     * @param flat Indicates whether value should be flattened, so
     * that the value for a single iteration and single chain is a
     * vector.
     */
    SArray dump(bool flat = false) const;
    /**
     * Returns the names of individual elements of the monitored value.
     * The default implementation copies these from the element names
     * of the stat.
     */
    virtual std::vector<std::string> elementNames() const;
    /**
     * Returns a vector of length equal to the length of Monitor#dim giving
     * the names of the corresponding dimension. These names may be null,
     * represented by empty vectors.
     *
     * The default implementation returns non-null dimnames only in the
     * case that the unit monitored value is a vector, when dimnames
     * are the same as elementNames.
     */
    virtual std::vector<std::vector<std::string>> dimNames() const;
    /**
     * Sets the names attribute of the stat. This is used to construct
     * the element names of the monitor
     */
    void setStatNames(std::vector<std::string> const &names);
    
};

} /* namespace jags */

#endif // MONITOR_H_
