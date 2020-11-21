//
//  EulerNode.hpp
//  
//
//  Created by Claire Cheng on 10/12/20.
//

#ifndef EULER_NODE_H
#define EULER_NODE_H

#include <stdio.h>
#include "ForwardEulerIntegrator.hpp"
#include "IntegratorBase.hpp"

#include <stdexcept>
#include "gloo/utils.hpp"
#include "IntegratorType.hpp"
#import "SimpleSystem.hpp"
#import "ParticleState.hpp"

#include <algorithm>
#include <iostream>

#include "gloo/SceneNode.hpp"

namespace GLOO {

class EulerNode : public SceneNode {
  public:
    EulerNode(IntegratorType integrator_type, ParticleState& state, SimpleSystem& system, float integration_step);
    
  void Update(double delta_time) override;
    
  private:
    IntegratorBase<SimpleSystem, ParticleState> *integrator_;
    ParticleState& state_;
    SimpleSystem& system_;
    float integration_step_;
};
}

#endif /* EulerNode_hpp */
