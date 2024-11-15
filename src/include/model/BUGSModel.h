#ifndef BUGS_MODEL_H_
#define BUGS_MODEL_H_

#include <vector>
#include <map>
#include <utility>
#include <string>

#include <model/Model.h>
#include <model/SymTab.h>
//#include <model/MonitorInfo.h> FIXME

namespace jags {

/**
 * @short Model with symbol table 
 *
 * A BUGS model is a subclass of Model that contains a symbol table
 * which is used to store certain nodes in arrays.  The array format
 * gives a convenient way of looking up nodes by name.
 */
class BUGSModel : public Model
{
    SymTab _symtab;
    //std::map<Node const*, std::pair<std::string, Range> > _node_map;
    //std::list<MonitorInfo> _bugs_monitors; FIXME

    // Only to be used by observedStochasticNodes():
    std::vector<Node const *> _observed_stochastic_nodes;

public:
    BUGSModel(unsigned int nchain);
    ~BUGSModel() override;
    /**
     * Returns the symbol table of the BUGSModel.
     */
    SymTab &symtab();
    /**
     * Writes out selected monitors in CODA format.
     *
     * @param nodes Vector of monitored nodes to write out. Each node
     * is described by a pair consisting of a name and a range of
     * indices.  If a node is not being monitored, then it is ignored.
     *
     * @param prefix String giving prefix to be prepened to the output
     * file names.
     *
     * @param warn String that will contain any warning messages on
     * exit. It is cleared on entry.
     * 
     * @param stat Select Monitors of the given stat, or "*" for all
     * stats.
     *
     * @param summary Select Monitors with the given summary, or "*"
     * for all summaries.
     *
     * @exception logic_error
     */
    void coda(std::vector<std::pair<std::string,Range> > const &nodes, 
	      std::string const &prefix, std::string &warn,
	      std::string const &stat, std::string const &summary);
    /**
     * Write out all monitors in CODA format
     */
    void coda(std::string const &prefix, std::string &warn,
	      std::string const &stat, std::string const &summary);
    /**
     * Sets the state of the RNG, and the values of the unobserved
     * stochastic nodes in the model, for a given chain.
     *
     * @param param_table STL map, in which each entry relates a
     * variable name to an SArray.  If the name is ".RNG.state" or
     * ".RNG.seed", then the SArray is used to set the state of the RNG.
     * Otherwise the SArray value is used to set the unobserved
     * stochastic nodes in the model. Each SArray must have the same
     * dimensions as the NodeArray in the symbol table with the
     * corresponding name. Elements of the SArray must be set to the
     * missing value, JAGS_NA, unless they correspond to an element of
     * an unobserved StochasticNode.
     *
     * @param chain Number of chain (starting from zero) for which
     * parameter values should be set.
     *
     * @see RNG#init RNG#setState
     * @exception runtime_error
     */
    void setParameters(std::map<std::string, SArray> const &param_table,
		       unsigned int chain);
    /**
     * Creates a new Monitor. The BUGSModel is responsible for the
     * memory management of any monitor created this way. It is not
     * possible to create two monitors with the same name, range,
     * stat, and summary.
     *
     * @param name Name of the node array
     *
     * @param range Subset of indices of the node array defining hte
     * node to be monitored.
     * 
     * @param thin Thinning interval for monitor
     *
     * @param stat Statistic to monnitor
     *
     * @param summary How to summarize the monitored values
     *
     * @param msg User-friendly error message that may be given if no
     * monitor can be created.
     *
     * @return True if the monitor was created.  
     */
    bool setMonitor(std::string const &name, Range const &range,
		    unsigned int thin, std::string const &stat,
		    std::string const &summary,
		    std::string &msg);
    /**
     * Deletes a Monitor that has been previously created with a call
     * to setMonitor.
     *
     * @return True if the monitor was deleted.
     */
    bool deleteMonitor(std::string const &name, Range const &range,
		       std::string const &stat, std::string const &summary);
    /**
     * Traverses the list of monitor factories requesting default
     * monitors of the given stat and summary. The function returns
     * true after the first monitor factory has added at least one
     * node to the monitor list. If none of the available monitor
     * factories can create default monitors of the given type, the
     * return value is false.
     *
     * @see MonitorFactory#addDefaultMonitors
     */
    //FIXME: Do we need this?
    bool setDefaultMonitors(std::string const &stat,
			    std::string const &summary,
			    unsigned int thin);
    /**
     * Removes all Monitors with the given stat and summary.
     */
    void clearMonitors(std::string const &stat,
		       std::string const &summary);
    /**
     * Writes the names of the samplers, and the corresponding 
     * sampled nodes vectors to the given vector.
     *
     * @param sampler_names vector that is modified during the call On
     * exit it will contain an element for each Sampler in the model.
     * Each element is a vector of strings: the first string is the
     * name of the sampler, and the remaining strings are the names of
     * the nodes sampled by that Sampler.
     */
    void samplerNames(std::vector<std::vector<std::string> > &sampler_names) 
	const;

    /**
     * Returns a vector of all observed stochastic nodes in the model
     */ 
    std::vector<Node const *> const &observedStochasticNodes();
    /**
     * Retrieves the names of nodes in the graph matching a given type
     *
     * @param node_names Node names will be appended to this vector
     *
     * @param type String indicating which type of nodes to select. Possible
     * values are "constant", "deterministic", "stochastic", "fixed", and "observed".
     *
     * @param warn String that will contain a warning if the type argument is not
     * matched with any valid string.
     * 
     * If type="observed" then these names are guaranteed to
     * correspond to the values given by deviance monitors in the DIC module
     */
    void dumpNodeNames(std::vector<std::string> &node_names,
		       std::string const &type, std::string &warn) const;
};

} /* namespace jags */

#endif /* BUGS_MODEL_H_ */
