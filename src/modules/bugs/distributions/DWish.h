#ifndef DWISH_H_
#define DWISH_H_

#include <distribution/ArrayDist.h>

namespace jags {
namespace bugs {

/**
 * <pre>
 * x[] ~ dwish(R[,], k)
 * </pre>
 * @short Wishart distribution
 */
class DWish : public ArrayDist {
public:
  DWish();

  double logDensity(double const *x, PDFType type,
		    std::vector<double const *> const &parameters,
		    std::vector<std::vector<unsigned long> > const &dims)
      const override;
  void randomSample(double *x,
		    std::vector<double const *> const &parameters,
		    std::vector<std::vector<unsigned long> > const &dims,
		    RNG *rng) const override;
  /**
   * Checks that R is a square matrix and k is a scalar
   */
  bool checkParameterDim(std::vector<std::vector<unsigned long> > const &dims) 
      const override;
  /**
   * Checks that R is symmetric and k >= nrow(R). There is
   * currently no check that R is positive definite
   */
  bool checkParameterValue(std::vector<double const *> const &parameters,
			   std::vector<std::vector<unsigned long> > const &dims)
      const override;
  std::vector<unsigned long> 
      dim(std::vector<std::vector<unsigned long> > const &dims) const override;
  void support(double *lower, double *upper,
	       std::vector<double const *> const &parameters,
	       std::vector<std::vector<unsigned long> > const &dims)
      const override;
  bool isSupportFixed(std::vector<bool> const &fixmask) const override;
  bool fullRank() const override;
  /**
   * Utility function for drawing a random sample from a Wishart distribution.
   * Used by Conjugate Wishart sampler
   */
  static void rwishart(double *x,
                       double const *R, double k, unsigned long nrow,
                       RNG *rng);
    
};

}}

#endif /* DWISH_H_ */
