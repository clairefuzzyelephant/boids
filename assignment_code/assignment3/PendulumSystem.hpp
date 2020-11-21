//
//  PendulumSystem.hpp
//  
//
//  Created by Claire Cheng on 10/14/20.
//

#ifndef PENDULUM_SYSTEM_HPP
#define PENDULUM_SYSTEM_HPP

#include <stdio.h>
#include "ParticleSystemBase.hpp"
#include "SpringInfo.hpp"

#include "IntegratorBase.hpp"
#include "glm/gtx/string_cast.hpp"

#include <stdexcept>

#include "gloo/utils.hpp"

#include <iostream>
#include <tuple>

namespace GLOO {

class PendulumSystem : public ParticleSystemBase {
public:
  PendulumSystem() {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    state_ = {positions, velocities};
    
  }
  
  ParticleState state_;
  std::vector<SpringInfo> spring_pairs_;
  mutable bool wind_on_ = false;
  
  virtual ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const {
    float mass = 5.f;
    std::vector<glm::vec3> accels;
    for (int i = 0; i < state.positions.size(); i++) {
      if (i == 0) {
        // base fixed particle
        accels.push_back(glm::vec3(0.f, 0.f, 0.f));
        continue;
      }
      glm::vec3 gravity_force = {0.f, mass * (-9.8), 0.f};
      glm::vec3 drag_force = {-0.5f * state.velocities[i][0], -0.5f*state.velocities[i][1], -0.5f * state.velocities[i][2]};
      glm::vec3 spring_force = {0.f, 0.f, 0.f};
      for (auto spring : spring_pairs_) {
        if (spring.child_index == i) {
          glm::vec3 d = state.positions[spring.parent_index] - state.positions[i];
          float mag_d = glm::sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
          spring_force = spring_force + spring.stiffness * (mag_d - spring.rest_distance) * (glm::normalize(d));
        }
        else if (spring.parent_index == i) {
          glm::vec3 d = state.positions[spring.child_index] - state.positions[i];
          float mag_d = glm::sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
          spring_force = spring_force + spring.stiffness * (mag_d - spring.rest_distance) * (glm::normalize(d));
        }
      }
      glm::vec3 total_accel = (1.f/mass) * (gravity_force + drag_force + spring_force);
      accels.push_back(total_accel);
    }
    
    struct ParticleState deriv_state;
    deriv_state.positions = state.velocities;
    deriv_state.velocities = accels;
    return deriv_state;
  }
  
  
  void AddParticle(bool is_fixed) {
    if (!is_fixed) {
      struct SpringInfo spring_info;
      spring_info.parent_index = state_.positions.size()-1;
      spring_info.child_index = state_.positions.size();
      spring_info.rest_distance = 0.15f;
      spring_info.stiffness = 250.f;
      spring_pairs_.push_back(spring_info);
      auto parent_pos = state_.positions[state_.positions.size()-1];
      state_.positions.push_back(glm::vec3(parent_pos[0], parent_pos[1] - 0.15f, parent_pos[2]));
      state_.velocities.push_back(glm::vec3(0.3f, -0.1f, 0.3f));
    }
    else {
      state_.positions.push_back(glm::vec3(-1.f, 0.f, 0.f));
      state_.velocities.push_back(glm::vec3(0.f, 0.f, 0.f));
    }
  }
};
}

#endif /* PendulumSystem_hpp */
