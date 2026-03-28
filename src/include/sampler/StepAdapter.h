#ifndef STEP_ADAPTER_H_
#define STEP_ADAPTER_H_

#include <sampler/Metropolis.h>

namespace jags {

/**
 * @short Step size for Random Walk Metropolis-Hastings
 *
 * Uses a stochastic gradient algorithm to adapt the step size of a
 * random walk Metropolis-Hastings algorithm to reach the target
 * acceptance probability. 
 */
class StepAdapter 
{
    const double _a;
    const double _delta; 
    double _theta0;
    double _theta;
    const double _min_step;
    unsigned long _n;
    const double _nstart0;
    double _nstart;

public:
    /**
     * Constructor. 
     *
     * @param step Initial step size for the random walk updates.
     *
     * @param accept Target acceptance probability
     *
     * @param delta Step size for the stochastic gradient algorithm is delta/n where n is the iteration
     *              number starting from nstart
     *
     * @param nstart Initial value of n, the denominator that determines the step size of the stochastic
     *               gradient algorithm
     *
     * @param min_step Minimal step size for random walk updates
     */
    StepAdapter(double step, double accept=0.234, double delta = 2.17, double nstart = 28, double min_step=0);
    /**
     * Recalculates the step size.
     *
     * @param p acceptance probability of current proposal
     */
    void rescale(double p);
    /**
     * Returns the current step size
     */
    double stepSize() const;
    /**
     * Returns the distance, on a logistic scale, between the argument
     * p and the target acceptance probability.
     */
    double logitDeviation(double p) const;
};

} /* namespace jags */

#endif /* STEP_ADAPTER_H_ */
