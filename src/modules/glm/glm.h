#ifndef GLM_H_
#define GLM_H_

#include <cholmod.h>

namespace jags {
    namespace glm {

	/**
	 * CHOLMOD's memory management uses a struct cholmod_common.
	 * In JAGS 4.x.y there was one cholmod_common structure owned
	 * by the GLM module. However, this is not compatible with paralel
	 * processing as it leads to race conditions when different
	 * threads share the same cholmod_common struct.
	 *
	 * In JAGS 5.0.0 this has been replaced by separate workspaces
	 * indexed by chain number n. If a workspace does not exist
	 * for the given n, a new one will be dynamically allocated
	 * and initialized. All workspaces are managed by the glm
	 * module.
	 *
	 * @param n Chain number (starting from zero)
	 **/
	cholmod_common *workspace(unsigned long n);
    }
}

#endif /* GLM_H_ */
