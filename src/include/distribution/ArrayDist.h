#ifndef ARRAY_DIST_H_
#define ARRAY_DIST_H_

#include <distribution/Distribution.h>

#include <vector>
#include <string>

namespace jags {

struct RNG;

/**
 * @short Matrix- or array-valued distribution
 *
 * This is the most general sub-class for distributions and is used
 * whenever a distribution takes values in a matrix or
 * higher-dimensional array (e.g. Wishart) or has parameters that are
 * array-valued (e.g. multivariate normal).
 */
class ArrayDist : public Distribution
{
public:
    /**
     * Constructor.
     * @param name name of the distribution as used in the BUGS language
     * @param npar number of parameters
     */
    ArrayDist(std::string const &name, unsigned int npar);
    /**
     * @param x Value at which to evaluate the density.
     *
     * @param type Indicates whether the full probability density
     * function is required (PDF_FULL) or whether partial calculations
     * are permitted (PDF_PRIOR, PDF_LIKELIHOOD). See PDFType for
     * details.
     *
     * @param parameters Vector of parameter values of the
     * distribution.
     * 
     * @param dims Dimensions of the parameters.
     *
     * @returns The log probability density.  If the density should be
     * zero because x is inconsistent with the parameters then -Inf is
     * returned. If the parameters are invalid
     * (i.e. checkParameterValue returns false), then the return value
     * is undefined.
     */
    virtual double 
    logDensity(double const *x, PDFType type,
	       std::vector<double const *> const &parameters,
	       std::vector<std::vector<unsigned long> > const &dims)
	const = 0;
    /**
     * Calculates the score function.
     *
     * @param s Array to hold the results (assumed to be of correct
     * length). The score function is added to this array.
     *
     * @param x Value at which to evaluate the score function (assumed
     * to be of the correct length).
     *
     * @param parameters Vector of parameter values.
     * 
     * @param dims Vector of parameter dimensions.
     *
     * @param i Index of the parameter for which the score function is
     * calculated. This function should only be called if
     * Distribution#hasScore returns true for the same value of i.
     * Otherwise the effect is undefined.
     */
    virtual void score(double *s, double const *x,
		       std::vector<double const *> const &parameters,
		       std::vector<std::vector<unsigned long>> const &dims,
		       unsigned long i) const;
    /**
     * Draws a random sample from the distribution. 
     *
     * @param x Array to which the sample values are written
     *
     * @param parameters Parameters for the distribution. This vector
     * should be of length npar().  Each element is a pointer to the
     * start of an array containing the parameters. The size of the 
     * array should correspond to the dims parameter. 
     *
     * @param dims Dimensions of the parameters
     *
     * @param rng pseudo-random number generator to use.
     *
     * @exception length_error 
     */
    virtual void 
	randomSample(double *x,
		     std::vector<double const *> const &parameters,
		     std::vector<std::vector<unsigned long> > const  &dims,
		     RNG *rng) 	const = 0;
    /**
     * Checks that dimensions of the parameters are correct.
     */
    virtual bool 
	checkParameterDim (std::vector<std::vector<unsigned long> > const &parameters) 
	const = 0;
    /**
     * Checks that the values of the parameters are consistent with
     * the distribution. For example, some distributions require than
     * certain parameters are positive, or lie in a given range.
     *
     * This function assumes that checkParameterDim returns true.
     */
    virtual bool 
	checkParameterValue(std::vector<double const *> const &parameters,
			    std::vector<std::vector<unsigned long> > const &dims) const = 0;
    /**
     * Calculates what the dimension of the distribution should be,
     * based on the dimensions of its parameters. 
     */
    virtual std::vector<unsigned long> 
	dim (std::vector <std::vector<unsigned long> > const &args) const = 0;
    /**
     * Returns the support of an unbounded distribution
     */
    virtual void 
	support(double *lower, double *upper,
		std::vector<double const *> const &support,
		std::vector<std::vector<unsigned long> > const &dims) const = 0;
    /**
     * Returns a Monte Carlo estimate of the Kullback-Leibler
     * divergence between distributions with two different parameter
     * values. This is done by drawing random samples from the
     * distribution with the first set of parameters and then
     * calculating the log-likelihood ratio with respect to the second
     * set of parameters.
     *
     * A subclass of ArrayDist can overload this function if the
     * Kullback-Leibler divergence for the distribution it represents
     * can be expressed in closed form.
     *
     * @param par1 First set of parameters
     * @param par2 Second set of parameter values
     * @param dims Vector of parameter dimensions, common to both par1 and par2
     * @param rng Random number generator
     * @param nrep Number of replicates on which to base the estimate
     */
    double KL(std::vector<double const *> const &par1,
	      std::vector<double const *> const &par2,
	      std::vector<std::vector<unsigned long> > const &dims,
	      RNG *rng, unsigned int nrep) const;
    /**
     * Returns the Kullback-Leibler divergence between distributions
     * with two different parameter values.
     *
     * This is a virtual function that must be overloaded for any
     * distribution that allows exact Kullback-Leibler divergence
     * calculations. The default method returns JAGS_NAN, indicating that
     * the method is not implemented.
     */
    virtual double KL(std::vector<double const *> const &par1,
		      std::vector<double const *> const &par2,
		      std::vector<std::vector<unsigned long> > const &dims)
	const;
    /**
     * Draws a random sample from the distribution conditional on some
     * elements being fixed.
     *
     * @param x Array to which the sample values are written
     *
     * @param parameters Parameters for the distribution. This vector
     * should be of length npar().  Each element is a pointer to the
     * start of an array containing the parameters. The size of the 
     * array should correspond to the dims parameter. 
     *
     * @param dims Dimensions of the parameters
     *
     * @param rng pseudo-random number generator to use.
     *
     * The default implementation throws an exception. A distribution
     * that allows partially observed values (i.e. isPartObservable
     * returns true) must overload the default implemention.
     */
    virtual void randomSample(double *x, std::vector<bool> const &fixed,
			      std::vector<double const *> const &parameters,
			      std::vector<std::vector<unsigned long>> const &dims, 
			      RNG *rng) const;
    /**
     * Calculates the conditional log density of fixed parts of
     * the value conditional on non-fixed parts. For a fully fixed
     * node this should be equal to logDensity with type=PDF_FULL.
     *
     * @param x Value at which to evaluate the conditional density
     * (assumed to be of the correct length).
     *
     * @param observed Boolean mask indicating which elements of x are
     * observed (true) and which are unobserved (false).
     * 
     * @param parameters Vector of parameter values of the
     * distribution.
     *
     * @param dims Vector of parameter dimensions.
     *
     * The default implementation returns JAGS_NAN, indicating that the
     * method is not implemented.
     */
    virtual double 
    logLikelihood(double const *x, std::vector<bool> const &observed,
		  std::vector<double const *> const &parameters,
		  std::vector<std::vector<unsigned long>> const &dims) const;

};

} /* namespace jags */

#endif /* ARRAY_DIST_H_ */
