#ifndef FORWARD_EULER_INTEGRATOR_H_
#define FORWARD_EULER_INTEGRATOR_H_

#include "IntegratorBase.hpp"
#include "ParticleSystemBase.hpp"
#include <iostream>
#include "glm/gtx/string_cast.hpp"

namespace GLOO {
template <class TSystem, class TState>
class ForwardEulerIntegrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
    auto temp = system.ComputeTimeDerivative(state, start_time);
    
    auto new_state = state + (dt * temp);
    
    return new_state;
  }
};
}  // namespace GLOO

#endif
