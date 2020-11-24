//
//  SimpleSystem.hpp
//  
//
//  Created by Claire Cheng on 10/12/20.
//

#ifndef SIMPLE_SYSTEM_HPP
#define SIMPLE_SYSTEM_HPP

#include <stdio.h>
#include "ParticleSystemBase.hpp"

#include "IntegratorBase.hpp"

#include <stdexcept>

#include "gloo/utils.hpp"

#include <iostream>

namespace GLOO {

class SimpleSystem : public ParticleSystemBase {
public:
  SimpleSystem() {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    positions.push_back(glm::vec3(-0.3f, 0.6f, 0.f));
    velocities.push_back(glm::vec3(0.f, 0.f, 0.f));
    state_ = {positions, velocities};
  }
  
  ParticleState state_;
  mutable bool wind_on_ = false;
  
  virtual ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const {
    std::vector<glm::vec3> positions;
    positions.push_back(glm::vec3(-state.positions[0].y,state.positions[0].x, 0));
    struct ParticleState deriv_state;
    deriv_state.positions = positions;
    deriv_state.velocities = state.velocities;
    return deriv_state;
  }
};
}

#endif /* SimpleSystem_hpp */
