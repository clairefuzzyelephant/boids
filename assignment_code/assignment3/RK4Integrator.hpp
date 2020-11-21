//
//  RK4Integrator.hpp
//  
//
//  Created by Claire Cheng on 10/16/20.
//
#ifndef RK4_INTEGRATOR_H_
#define RK4_INTEGRATOR_H_


#include "IntegratorBase.hpp"
#include "ParticleSystemBase.hpp"
#include <iostream>

namespace GLOO {
template <class TSystem, class TState>
class RK4Integrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
    auto k1 = system.ComputeTimeDerivative(state, start_time);
    auto k2 = system.ComputeTimeDerivative(state + (dt/2.f) * k1, start_time + (dt/2.f));
    auto k3 = system.ComputeTimeDerivative(state + (dt/2.f) * k2, start_time + (dt/2.f));
    auto k4 = system.ComputeTimeDerivative(state + (dt * k3), start_time + dt);
    
    return state + (dt/6.0) * (k1 + 2.f*k2 + 2.f*k3 + k4);
  }
};
}  // namespace GLOO

#endif
