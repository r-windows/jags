#include <config.h>

#include "Det.h"

#include <util/dim.h>
#include <util/integer.h>
#include <matrix/matrix.h>

#include <cmath>

using std::vector;

namespace jags {
namespace bugs {

    Det::Det ()
	: ArrayFunction ("det", 1)
    {
    }

    void Det::evaluate (double *x, vector<double const *> const &args,
			vector<vector<unsigned long> > const &dims) const
    {
	*x = det(args[0], dims[0][0]);
    }

    bool 
    Det::checkParameterDim (vector<vector<unsigned long> > const &dims) const
    {
	return isSquareMatrix(dims[0]) || isScalar(dims[0]);
    }

    vector<unsigned long> Det::dim(vector<vector<unsigned long> > const &,
				   vector<double const *> const &) const
    {
	return vector<unsigned long>(1,1);
    }

}}

