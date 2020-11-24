//
//  ClothSystem.hpp
//  
//
//  Created by Claire Cheng on 10/16/20.
//

#ifndef CLOTH_SYSTEM_HPP
#define CLOTH_SYSTEM_HPP

#include <stdio.h>
#include "ParticleSystemBase.hpp"
#include "SpringInfo.hpp"

#include "IntegratorBase.hpp"
#include "glm/gtx/string_cast.hpp"

#include "gloo/InputManager.hpp"

#include <stdexcept>

#include "gloo/utils.hpp"

#include <iostream>
#include <tuple>

namespace GLOO {

class ClothSystem : public ParticleSystemBase {
public:
  ClothSystem() {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;

    
    //adding particles
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        if (i == 0) {
          positions.push_back(glm::vec3(j * 0.1f, 0.f, 0.f));
          velocities.push_back(glm::vec3(0.f, 0.f, 0.f));
        }
        else {
          positions.push_back(glm::vec3(j * 0.1f, -i * 0.1f, 0.f));
          velocities.push_back(glm::vec3(0.1f, -0.01f, 0.1f));
        }
      }
    }
    state_ = {positions, velocities};
        
    //adding structural springs, horizontal
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 7; j++) {
        struct SpringInfo new_spring;
        new_spring.parent_index = i*8 + j;
        new_spring.child_index = i*8 + j + 1;
        new_spring.rest_distance = 0.1f;
        new_spring.stiffness = 300.f;
        spring_pairs_.push_back(new_spring);
      }
    }
    
    //adding structural springs, vertical
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 8; j++) {
        struct SpringInfo new_spring;
        new_spring.parent_index = i*8 + j;
        new_spring.child_index = (i+1)*8 + j;
        new_spring.rest_distance = 0.1f;
        new_spring.stiffness = (11-i) * 100.f;
        spring_pairs_.push_back(new_spring);
      }
    }
    
    //adding shear springs, left-right diagonals
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 7; j++) {
        struct SpringInfo new_spring;
        new_spring.parent_index = i*8 + j;
        new_spring.child_index = (i+1)*8 + j+1;
        new_spring.rest_distance = 0.1f * glm::sqrt(2.f);
        new_spring.stiffness = 200.f;
        spring_pairs_.push_back(new_spring);
      }
    }
    
    //adding shear springs, right-left diagonals
    for (int i = 0; i < 7; i++) {
      for (int j = 1; j < 8; j++) {
        struct SpringInfo new_spring;
        new_spring.parent_index = i*8 + j;
        new_spring.child_index = (i+1)*8 + j-1;
        new_spring.rest_distance = 0.1f * glm::sqrt(2.f);
        new_spring.stiffness = 200.f;
        spring_pairs_.push_back(new_spring);
      }
    }
    
    //adding flex springs springs, horizontal
    for (int i = 0; i < 8; i++) {
      for (int j = 1; j < 6; j++) {
        struct SpringInfo new_spring;
        new_spring.parent_index = i*8 + j;
        new_spring.child_index = i*8 + j+2;
        new_spring.rest_distance = 0.2f;
        new_spring.stiffness = 200.f;
        spring_pairs_.push_back(new_spring);
      }
    }
    
    //adding flex springs springs, vertical
    for (int i = 0; i < 6; i++) {
      for (int j = 1; j < 8; j++) {
        struct SpringInfo new_spring;
        new_spring.parent_index = i*8 + j;
        new_spring.child_index = (i+2)*8 + j;
        new_spring.rest_distance = 0.2f;
        new_spring.stiffness = 200.f;
        spring_pairs_.push_back(new_spring);
      }
    }
    
    
  }
  
  ParticleState state_;
  std::vector<SpringInfo> spring_pairs_; //tuple stores: parent index, child index, rest distance, spring stiffness
  mutable bool wind_on_ = false;
  
  virtual void ToggleWind() const override {
    wind_on_ = !wind_on_;
  }
  
  
  virtual ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const override {
    
    //activate/de-activate wind force
    static bool prev_released = true;
    if (InputManager::GetInstance().IsKeyPressed('W')) {
      if (prev_released) {
        this->ToggleWind();
      }
      prev_released = false;
    } else if (InputManager::GetInstance().IsKeyReleased('W')) {
      prev_released = true;
    }
    
    float mass = 1.f;
    std::vector<glm::vec3> accels;
    for (int i = 0; i < state.positions.size(); i++) {
      if (i < 8) {
        //top row fixed particles
        accels.push_back(glm::vec3(0.f, 0.f, 0.f));
        continue;
      }
      glm::vec3 gravity_force = {0.f, mass * (-9.8), 0.f};
      glm::vec3 drag_force = {-2.5f * state.velocities[i][0], -2.5f*state.velocities[i][1], -2.5f * state.velocities[i][2]};
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
      // wind force in the +x and -z direction
      glm::vec3 wind_force;
      if (wind_on_) {
         wind_force = {0.613*100.f*2.f * 0.01f, 0.f, -0.613*100.f*2.f * 0.05f};
      }
      else {
        wind_force = {0.f, 0.f, 0.f};
      }
      
      glm::vec3 total_accel = (1.f/mass) * (gravity_force + drag_force + spring_force + wind_force);
      accels.push_back(total_accel);
    }
    
    struct ParticleState deriv_state;
    deriv_state.positions = state.velocities;
    deriv_state.velocities = accels;
    return deriv_state;
  }
};
}

#endif /* ClothSystem_hpp */
