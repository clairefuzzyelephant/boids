//
//  EulerNode.cpp
//  
//
//  Created by Claire Cheng on 10/12/20.
//

#include "EulerNode.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "TrapezoidalIntegrator.hpp"
#include "RK4Integrator.hpp"
#import "ParticleState.hpp"
#import "SimpleSystem.hpp"
#import "IntegratorType.hpp"

#include "glm/gtx/string_cast.hpp"

#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"

#include <iostream>


namespace GLOO {

EulerNode::EulerNode(IntegratorType integrator_type, ParticleState& state, SimpleSystem& system, float integration_step) : state_(state), system_(system)
{
  
  std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
  this->CreateComponent<ShadingComponent>(shader);
  std::shared_ptr<VertexObject> mesh = PrimitiveFactory::CreateSphere(0.1f, 25, 25);
  this->CreateComponent<RenderingComponent>(mesh);
  this->GetTransform().SetPosition(state_.positions[0]);
  this->CreateComponent<MaterialComponent>( std::make_shared<Material>(Material::GetDefault()));
  
  integration_step_ = integration_step;
  
  if (integrator_type == IntegratorType::Euler) {
    integrator_ = new ForwardEulerIntegrator<SimpleSystem, ParticleState>();
  }
  else if (integrator_type == IntegratorType::Trapezoidal) {
    integrator_ = new TrapezoidalIntegrator<SimpleSystem, ParticleState>();
  }
  else {
    integrator_ = new RK4Integrator<SimpleSystem, ParticleState>();
  }
  
}

void EulerNode::Update(double delta_time) {
  float steps = int(delta_time/integration_step_) > 0 ? int(delta_time/integration_step_) : delta_time;
  for (int i = 0; i < steps; i++) {
    auto new_state = integrator_->Integrate(system_, state_, 0.0, integration_step_);
    this->GetTransform().SetPosition(new_state.positions[0]);
    state_.positions[0] = new_state.positions[0];
  }
}
}
