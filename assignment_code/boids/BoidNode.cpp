#include "BoidNode.hpp"

#include <map>
#include "gloo/MeshLoader.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"

namespace GLOO {
BoidNode::BoidNode(const std::string& filename, const glm::vec3 position) : SceneNode() {
  LoadMeshFile(filename);
  shader_ = std::make_shared<PhongShader>();

  auto mesh_node = make_unique<SceneNode>();
  mesh_node->CreateComponent<ShadingComponent>(shader_);
  mesh_node->CreateComponent<RenderingComponent>(mesh_);
  auto mat = mesh_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  mat.GetMaterial().SetDiffuseColor(glm::vec3(197, 172, 112)/255.f);
  mesh_node->SetActive(true);
  mesh_node_ = mesh_node.get();
  AddChild(std::move(mesh_node));
  mesh_node_->GetTransform().SetScale(glm::vec3(0.01));
  mesh_node_->GetTransform().SetPosition(position);
  
  
  position_ = position;
  velocity_ = glm::vec3(rand()%3-2.f, rand()%3-2.f, rand()%3-2.f);
  acceleration_ = glm::vec3(0.f, 0.f, 0.f);
  
  max_speed_ = 3.f;
  max_force_ = 1.f;
}

void BoidNode::UpdateBoids(double delta_time) {
  // TODO: implement rules and call them here
  acceleration_ = acceleration_ * 0.5f; //dampen
  velocity_ = velocity_ + acceleration_;
  position_ = position_ + velocity_;
  acceleration_ = glm::vec3(0.f, 0.f, 0.f);
}

void BoidNode::Run(const std::vector<BoidNode*>& boids)
{
    Flock(boids);
    UpdateBoids(0.001);
//    borders();
}

void BoidNode::Flock(const std::vector<BoidNode*>& boids) {
  glm::vec3 separation = Separation(boids);
  glm::vec3 alignment = Alignment(boids);
  glm::vec3 cohesion = Cohesion(boids);
  
  AddForce(separation);
  AddForce(alignment);
  AddForce(cohesion);
}

void BoidNode::AddForce(glm::vec3 force) {
  acceleration_ = acceleration_ + force;
}

// Separation
// Keeps boids from getting too close to one another
glm::vec3 BoidNode::Separation(const std::vector<BoidNode*>& boids)
{
  // Distance of field of vision for separation between boids
  float desiredseparation = 20;
  glm::vec3 steer(0.f, 0.f, 0.f);
  int count = 0;
  // For every boid in the system, check if it's too close
  for (uint i = 0; i < boids.size(); i++) {
      // Calculate distance from current boid to boid we're looking at
      float d = glm::distance(position_, boids[i]->position_);
      // If this is a fellow boid and it's too close, move away from it
      if ((d > 0) && (d < desiredseparation)) {
          glm::vec3 diff = position_ -  boids[i]->position_;
          glm::normalize(diff);
          diff = diff/d;
          steer = steer + diff;
          count++;
      }
      // If current boid is a predator and the boid we're looking at is also
      // a predator, then separate only slightly
//        if ((d > 0) && (d < desiredseparation) && predator == true
//            && boids[i].predator == true) {
//            Pvector pred2pred(0, 0);
//            pred2pred = pred2pred.subTwoVector(location, boids[i].location);
//            pred2pred.normalize();
//            pred2pred.divScalar(d);
//            steer.addVector(pred2pred);
//            count++;
//        }
      // If current boid is not a predator, but the boid we're looking at is
      // a predator, then create a large separation Pvector
//        else if ((d > 0) && (d < desiredseparation+70) && boids[i].predator == true) {
//            Pvector pred(0, 0);
//            pred = pred.subTwoVector(location, boids[i].location);
//            pred.mulScalar(900);
//            steer.addVector(pred);
//            count++;
//        }
  }
  // Adds average difference of location to acceleration
  if (count > 0)
      steer = steer / (float)count;

  float steer_mag = glm::length(steer);
  if (steer_mag > 0) {
      // Steering = Desired - Velocity
      steer = glm::normalize(steer);
      steer = steer * max_speed_;
      steer = steer - velocity_;
      if (steer_mag > max_force_) {
        steer = steer/steer_mag;
      }
  }
  return steer;
}

glm::vec3 BoidNode::Alignment(const std::vector<BoidNode*>& boids)
{
  float neighbordist = 50; // Field of vision

  glm::vec3 sum(0.f, 0.f, 0.f);
  int count = 0;
  for (uint i = 0; i < boids.size(); i++) {
      float d = glm::distance(position_, boids[i]->position_);
      if ((d > 0) && (d < neighbordist)) { // 0 < d < 50
          sum = sum + boids[i]->velocity_;
          count++;
      }
  }
  // If there are boids close enough for alignment...
  if (count > 0) {
      sum = sum /(float)count;
      glm::normalize(sum);
      sum = sum * max_speed_;
      glm::vec3 steer = sum - velocity_;
      float steer_mag = glm::length(steer);
      if (steer_mag > max_force_) {
        steer = steer/steer_mag;
      }
      return steer;
  } else {
      return glm::vec3(0.f, 0.f, 0.f);
  }
}


// Cohesion
// Finds the average location of nearby boids and manipulates the
// steering force to move in that direction.
glm::vec3 BoidNode::Cohesion(const std::vector<BoidNode*>& boids)
{
  float neighbordist = 50;
  glm::vec3 sum(0.f, 0.f, 0.f);
  int count = 0;
  for (uint i = 0; i < boids.size(); i++) {
      float d = glm::distance(position_, boids[i]->position_);
      if ((d > 0) && (d < neighbordist)) {
          sum = sum + boids[i]->position_;
          count++;
      }
  }
  if (count > 0) {
      sum = sum / (float)count;
      return seek(sum);
  } else {
      return glm::vec3(0.f, 0.f, 0.f);
  }
}

glm::vec3 BoidNode::seek(const glm::vec3 v)
{
  glm::vec3 desired = -v; //????
  // Normalize desired and scale to maximum speed
  glm::normalize(desired);
  desired = desired * max_speed_;
  // Steering = Desired minus Velocity
  acceleration_ = desired - velocity_;

  float accel_mag = glm::length(acceleration_);
  if (accel_mag > max_force_) {
    acceleration_ = acceleration_ / accel_mag;
  }
  return acceleration_;
}

void BoidNode::LoadMeshFile(const std::string& filename) {
  std::shared_ptr<VertexObject> vtx_obj =
      MeshLoader::Import(GetAssetDir() + filename).vertex_obj;
  mesh_ = vtx_obj;
  auto positions = mesh_->GetPositions();
  for (uint i = 0; i < positions.size(); i++)
    mesh_positions_.push_back(positions[i]);
}
}
