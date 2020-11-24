//
//  PendulumNode.hpp
//  
//
//  Created by Claire Cheng on 10/14/20.
//

#ifndef PENDULUM_NODE_HPP
#define PENDULUM_NODE_HPP

#include <stdio.h>
#include "ForwardEulerIntegrator.hpp"
#include "TrapezoidalIntegrator.hpp"
#include "IntegratorBase.hpp"
#include "ParticleSystemBase.hpp"

#include <stdexcept>
#include "gloo/utils.hpp"
#include "IntegratorType.hpp"
#import "PendulumSystem.hpp"
#import "ParticleState.hpp"

#include <algorithm>
#include <iostream>

#include "gloo/SceneNode.hpp"

#include "SpringInfo.hpp"

namespace GLOO {

class PendulumNode : public SceneNode {
  
  public:
    PendulumNode(IntegratorType integrator_type, ParticleState& state, ParticleSystemBase& system, float integration_step, std::vector<SpringInfo> spring_pairs);
  void Update(double delta_time) override;
  
  std::vector<SceneNode*> spheres_;
  
  
    
  private:
    IntegratorBase<ParticleSystemBase, ParticleState> *integrator_;
    ParticleState& state_;
    ParticleSystemBase& system_;
    float integration_step_;
    std::vector<SpringInfo> spring_pairs_;
    
};
}

#endif /* PendulumNode_hpp */
