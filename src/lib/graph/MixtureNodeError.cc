#include <config.h>
#include <graph/MixtureNodeError.h>
#include <graph/MixtureNode.h>
#include <model/SymTab.h>

using std::string;
using std::ostream;
using std::endl;

namespace jags {

    MixtureNodeError::MixtureNodeError(MixtureNode const *mixnode,
				       string const &msg, unsigned long chain)
	: NodeError(mixnode, msg), _mixnode(mixnode), _chain(chain)
    {
    }

    void
    MixtureNodeError::printMessage(ostream &out, SymTab const &symtab) const
    {
	out << "Error in mixture node " <<  symtab.getName(_mixnode) << "\n" 
	    << what() << "\n"
	    << "Index value: " << symtab.getMixtureIndex(_mixnode, _chain)
	    << endl;
    }

} //namespace jags
