//
//  PendulumNode.cpp
//  
//
//  Created by Claire Cheng on 10/14/20.
//

#include "PendulumNode.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "TrapezoidalIntegrator.hpp"
#include "RK4Integrator.hpp"
#import "ParticleState.hpp"
#import "ParticleSystemBase.hpp"
#import "PendulumSystem.hpp"
#import "ClothSystem.hpp"
#import "IntegratorType.hpp"

#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"

#include "gloo/shaders/SimpleShader.hpp"

#include "glm/gtx/string_cast.hpp"

#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/SceneNode.hpp"

#include "gloo/InputManager.hpp"

#include <iostream>


namespace GLOO {

PendulumNode::PendulumNode(IntegratorType integrator_type, ParticleState& state, ParticleSystemBase& system, float integration_step, std::vector<SpringInfo> spring_pairs) : state_(state), system_(system)
{
  for (int i = 0; i < state_.positions.size(); i++) {
    auto sphere = make_unique<SceneNode>();
    std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
    sphere->CreateComponent<ShadingComponent>(shader);
    std::shared_ptr<VertexObject> mesh = PrimitiveFactory::CreateSphere(0.02f, 25, 25);
    sphere->CreateComponent<RenderingComponent>(mesh);
    sphere->GetTransform().SetPosition(state_.positions[i]);
    sphere->CreateComponent<MaterialComponent>( std::make_shared<Material>(Material::GetDefault()));
    
    spheres_.push_back(sphere.get());
    AddChild(std::move(sphere));
  
  }
  
  auto line = std::make_shared<VertexObject>();
  auto shader = std::make_shared<SimpleShader>();
  
  for (int j = 0; j < spring_pairs.size(); j++) {
    auto spring = make_unique<VertexObject>();
    
    auto positions = make_unique<PositionArray>();
    positions->push_back(state_.positions[spring_pairs[j].parent_index]);
    positions->push_back(state_.positions[spring_pairs[j].child_index]);

    auto indices = make_unique<IndexArray>();
    indices->push_back(0);
    indices->push_back(1);
    
    spring->UpdatePositions(std::move(positions));
    spring->UpdateIndices(std::move(indices));
    
    auto spring_node = make_unique<SceneNode>();
    spring_node->CreateComponent<ShadingComponent>(shader);
    auto& rc = spring_node->CreateComponent<RenderingComponent>(std::move(spring));
    rc.SetDrawMode(DrawMode::Lines);

    glm::vec3 color(1.f, 1.f, 1.f);
    auto material = std::make_shared<Material>(color, color, color, 0);
    spring_node->CreateComponent<MaterialComponent>(material);
    
    AddChild(std::move(spring_node));
  }
  
  spring_pairs_ = spring_pairs;
  integration_step_ = integration_step;
  
  if (integrator_type == IntegratorType::Euler) {
    integrator_ = new ForwardEulerIntegrator<ParticleSystemBase, ParticleState>();
  }
  else if (integrator_type == IntegratorType::Trapezoidal) {
    integrator_ = new TrapezoidalIntegrator<ParticleSystemBase, ParticleState>();
  }
  else {
    integrator_ = new RK4Integrator<ParticleSystemBase, ParticleState>();
  }

}

void PendulumNode::Update(double delta_time) {
  
  int steps = int(delta_time/integration_step_) > 0 ? int(delta_time/integration_step_) : 1;
  auto new_state = integrator_->Integrate(system_, state_, 0.0, integration_step_);
  for (int i = 0; i < steps; i++) {
    auto new_state = integrator_->Integrate(system_, state_, 0.0, integration_step_);
    for (int j = 0; j < spheres_.size(); j++) {
      spheres_[j]->GetTransform().SetPosition(new_state.positions[j]);
      state_.positions[j] = new_state.positions[j];
      state_.velocities[j] = new_state.velocities[j];
    }
  }
  
  for (int i = 0; i < spring_pairs_.size(); i++) {
    auto spring = make_unique<VertexObject>();

    auto positions = make_unique<PositionArray>();
    positions->push_back(new_state.positions[spring_pairs_[i].parent_index]);
    positions->push_back(new_state.positions[spring_pairs_[i].child_index]);
    auto indices = make_unique<IndexArray>();
    indices->push_back(0);
    indices->push_back(1);

    GetChild(state_.positions.size() + i).GetComponentPtr<RenderingComponent>()->GetVertexObjectPtr()->UpdatePositions(std::move(positions));
    GetChild(state_.positions.size() + i).GetComponentPtr<RenderingComponent>()->GetVertexObjectPtr()->UpdateIndices(std::move(indices));

  }
}
}
