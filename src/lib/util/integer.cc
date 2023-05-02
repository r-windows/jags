#include <config.h>

#include <util/integer.h>

#include <cmath>
#include <limits>

using std::fabs;
using std::sqrt;
using std::round;
using std::numeric_limits;

namespace jags {

    static const unsigned long JAGS_ULONG_MAX = numeric_limits<unsigned long>::max();
    
    static const int JAGS_INT_MIN = numeric_limits<int>::min();
    static const int JAGS_INT_MAX = numeric_limits<int>::max();
    
    /*
      Numerical tolerance for values close to an integer. Following R
      we use the square root of the machine precision.
    */ 
    static const double JAGS_EPS = sqrt(numeric_limits<double>::epsilon());
  
    int asInteger(double fval)
    {
	if (fval < JAGS_INT_MIN) {
	    return JAGS_INT_MIN;
	}
	else if (fval > JAGS_INT_MAX) {
	    return JAGS_INT_MAX;
	}
	else {
	    return static_cast<int>(round(fval));
	}
    }

    bool checkInteger(double fval)
    {
	return fabs(fval - round(fval)) < JAGS_EPS;
    }
    
    unsigned long asULong(double fval)
    {
	if (fval < 0) {
	    return 0UL;
	}
	else if (fval > JAGS_ULONG_MAX) {
	    return JAGS_ULONG_MAX;
	}
	else {
	    return static_cast<unsigned long>(round(fval));
	}
    }
    
}
