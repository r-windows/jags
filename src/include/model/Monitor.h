#ifndef MONITOR_H_
#define MONITOR_H_

#include <sarray/SArray.h>
#include <sarray/Range.h>

#include <vector>
#include <string>

namespace jags {

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
    std::vector<std::string> _elt_names;
public:
    Monitor(std::vector<Node const *> const &nodes);
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
     * Returns the length of the monitored value corresponding to a
     * single iteration in a single chain.
     */
    virtual unsigned long length() const = 0;
    /**
     * Returns the dimensions of the value corresponding to a single
     * iteration in a single chain.
     */
    virtual std::vector<unsigned long> dim() const = 0;
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
     * Returns the names of individual elements, or an empty vector
     * if setElementNames has not been called.
     */
    std::vector<std::string> const &elementNames() const;
    /**
     * Sets the element names. The length of the string must be
     * match the length of the monitor as returned by the length
     * member function.
     */
    void setElementNames(std::vector<std::string> const &names);
};

} /* namespace jags */

#endif // MONITOR_H_
