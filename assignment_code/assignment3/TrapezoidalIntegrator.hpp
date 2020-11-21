#ifndef TRAPEZOIDAL_INTEGRATOR_H_
#define TRAPEZOIDAL_INTEGRATOR_H_

#include "IntegratorBase.hpp"
#include "ParticleSystemBase.hpp"
#include <iostream>

namespace GLOO {
template <class TSystem, class TState>
class TrapezoidalIntegrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
    auto td1 = system.ComputeTimeDerivative(state, start_time);
    auto state_1 = state + (dt * td1);
    
    auto td2 = system.ComputeTimeDerivative(state_1, start_time + dt);
    
    return state + (dt/2.0) * (td1 + td2);
  }
};
}  // namespace GLOO

#endif

