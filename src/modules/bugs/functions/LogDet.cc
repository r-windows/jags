#include <config.h>

#include "LogDet.h"

#include <util/dim.h>
#include <util/integer.h>
#include <matrix/matrix.h>
#include <module/ModuleError.h>

#include <cmath>

using std::vector;

namespace jags {
namespace bugs {

    LogDet::LogDet ()
	: ArrayFunction ("logdet", 1)
    {
    }

    void LogDet::evaluate (double *x, vector<double const *> const &args,
			   vector<vector<unsigned long> > const &dims) const
    {
	*x = logdet(args[0], dims[0][0]);
    }

    bool 
    LogDet::checkParameterDim (vector<vector<unsigned long> > const &dims) const
    {
	return isSquareMatrix(dims[0]) || isScalar(dims[0]);
    }

    vector<unsigned long> LogDet::dim(vector<vector<unsigned long> > const &,
				      vector<double const *> const &) const
    {
	return vector<unsigned long>(1,1);
    }

    
    void LogDet::gradient(double *grad, vector<double const *> const &args,
			  vector<vector<unsigned long>> const &dims,
			  unsigned long i) const
    {
	//FIXME Needs testing
	unsigned long nrow = dims[0][0];
	vector<double> work(nrow * nrow);
	bool can_invert = inverse_chol (work.data(), args[0], nrow);
	if (!can_invert) {
	    throwFuncError(this, "Cannot calculate gradient. Matrix may not be positive definite.");
	}
	for (unsigned long j = 0; j < nrow; ++j) {
	    grad[j*nrow + j] += work[j*nrow + j];
	    for (unsigned long k = 0; k < j; ++k) {
		grad[k*nrow + j] += 2 * work[k*nrow + j];
	    }
	}
    }
    
}}

