#include <config.h>
#include <sampler/RWMetropolis.h>
#include <rng/RNG.h>

#include <cmath>

using std::vector;
using std::log;
using std::exp;
using std::fabs;
using std::isfinite;
using std::min;

namespace jags {

RWMetropolis::RWMetropolis(vector<double> const &value, double step,
			   double a, double delta, double nstart, double min_step)
    : Metropolis(value), _step_adapter(step, a, delta, nstart, min_step), _niter(0)
{
    _psum[0] = 0.0;
    _psum[1] = 0.0;
}

RWMetropolis::~RWMetropolis()
{
}

void RWMetropolis::rescale(double p)
{
    p = min(p, 1.0);
    _step_adapter.rescale(p);

    /* The array _psum[2] holds a running total of the acceptance
       probabilities for the last 100-200 iterations. Every 100
       iterations we put the sum of the last 100 iterations in
       _psum[1] and start the sum again in _psum[0].
    */
    if (_niter / 100 > 0 && _niter % 100 == 0) {
	_psum[1] = _psum[0];
	_psum[0] = 0.0;

    }
    _psum[0] += p;
    _niter++;
}

void RWMetropolis::update(RNG *rng)
{
    vector<double> value(length());
    getValue(value);

    double log_p = logDensity() + logJacobian(value);
    step(value, _step_adapter.stepSize(), rng);
    setValue(value);
    double log_p_new = logDensity() + logJacobian(value);
    double odds = ( isfinite( log_p ) && isfinite( log_p_new ) )
                ?  exp( log_p_new - log_p )
                : ( log_p_new > log_p ? 1 : 0 );
    accept(rng, odds);
}

bool RWMetropolis::checkAdaptation() const
{
    if (_niter < 100) return false;

    /* Average acceptance probability over the last 100-200 iterations */
    double pmean = (_psum[0] + _psum[1])/(100 + (_niter % 100));
    
    return fabs(_step_adapter.logitDeviation(pmean)) < 0.5;
}

void RWMetropolis::step(vector<double> &value, double s, RNG *rng) const
{
    for (unsigned int i = 0; i < value.size(); ++i) {
	value[i] += rng->normal() * s;
    }
}

double RWMetropolis::logJacobian(vector<double> const &) const
{
    return 0;
}

} //namespace jags
