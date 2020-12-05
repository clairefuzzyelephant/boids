#include "BoidNode.hpp"

#include <map>
#include "gloo/MeshLoader.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"

#include "glm/gtx/string_cast.hpp"

namespace GLOO {
BoidNode::BoidNode(const std::string& filename, const glm::vec3 position) : SceneNode() {
  LoadMeshFile(filename);
  shader_ = std::make_shared<PhongShader>();
  auto offset = glm::vec3(-0.1, -0.03, -0.17);

  auto mesh_node = make_unique<SceneNode>();
  auto inner_node = make_unique<SceneNode>();
  inner_node->CreateComponent<ShadingComponent>(shader_);
  inner_node->CreateComponent<RenderingComponent>(mesh_);
  auto mat = inner_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  mat.GetMaterial().SetDiffuseColor(glm::vec3(197, 172, 112)/255.f);
  // Inner node necessary to fix .obj being slightly off center. Offset here fixes.
  inner_node->GetTransform().SetPosition(offset/2.5f);
  // If changing scale, may need to change offset proportionally
  inner_node->GetTransform().SetScale(glm::vec3(0.002));

  mesh_node->AddChild(std::move(inner_node));
  mesh_node_ = mesh_node.get();
  AddChild(std::move(mesh_node));
  mesh_node_->GetTransform().SetPosition(position);

  position_ = position;
  velocity_ = glm::vec3(0);
  acceleration_ = glm::vec3(0);
  
  max_speed_ = 5.f;
  max_force_ = 0.1f;
}

void BoidNode::UpdateBoids(double delta_time) {
  // TODO: implement rules and call them here
  acceleration_ = acceleration_ * 0.5f; //dampen
  velocity_ = velocity_ + (acceleration_ * (float)delta_time);
  if (glm::length(velocity_) > max_speed_) {
    velocity_ = glm::normalize(velocity_) * 5.f;
  }
  // std::cout << "velocity" << glm::to_string(velocity_) << std::endl;
  position_ = position_ + (velocity_ * (float)delta_time);
  mesh_node_->GetTransform().SetPosition(position_);

  glm::quat rot = glm::quat(glm::vec3(0, -3.14/2.f, 0)) * glm::quatLookAt(glm::normalize(velocity_), glm::vec3(0, 1, 0));
  mesh_node_->GetTransform().SetRotation(rot);

  acceleration_ = glm::vec3(0.f, 0.f, 0.f);
  
}

void BoidNode::Run(const std::vector<BoidNode*>& boids, double delta_time)
{
    Flock(boids, delta_time);
    UpdateBoids(delta_time);
    // std::cout << "position" << glm::to_string(position_) << std::endl;
}

void BoidNode::Flock(const std::vector<BoidNode*>& boids, double delta_time) {
  glm::vec3 separation = Separation(boids);
  glm::vec3 alignment = Alignment(boids);
  glm::vec3 cohesion = Cohesion(boids);
  glm::vec3 avoidance = Avoidance();
  
  AddForce(separation * (float)delta_time * 100.f);
  AddForce(alignment * (float)delta_time * 100.f);
  AddForce(cohesion * (float)delta_time * 300.f);
  AddForce(avoidance * (float)delta_time * 100.f);
}

void BoidNode::AddForce(glm::vec3 force) {
  acceleration_ = acceleration_ + force;
}

// Separation
// Keeps boids from getting too close to one another
glm::vec3 BoidNode::Separation(const std::vector<BoidNode*>& boids)
{
  // Distance of field of vision for separation between boids
  float desiredseparation = 0.2f;
  glm::vec3 steer(0.f, 0.f, 0.f);
  int count = 0;
  // For every boid in the system, check if it's too close
  for (uint i = 0; i < boids.size(); i++) {
      // Calculate distance from current boid to boid we're looking at
      float d = glm::distance(position_, boids[i]->position_);
      // If this is a fellow boid and it's too close, move away from it
      if ((d > 0) && (d < desiredseparation)) {
          glm::vec3 diff = position_ -  boids[i]->position_;
          if (glm::length(diff) > 0) {
            diff = glm::normalize(diff);
          }
          diff = diff/d;
          steer = steer + diff;
          count++;
      }
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
  float neighbordist = 50.f; // Field of vision

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
      if (glm::length(sum) > 0) {
        sum = glm::normalize(sum);
      }
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
  float neighbordist = 50.f;
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
    sum = sum / float(count);
    return seek(sum);
  } else {
    return glm::vec3(0);
  }
}

glm::vec3 BoidNode::Avoidance()
{
  glm::vec3 force = glm::vec3(0);

  float floor_y = -2;
  float ceiling_y = 2;
  if (position_.y < floor_y + 1) {
    force += glm::vec3(0, -position_.y/(position_.y-floor_y), 0);
  }
  if (position_.y > ceiling_y - 1) {
    force += glm::vec3(0, position_.y/(position_.y-ceiling_y), 0);
  }

  float right_x = 2;
  float left_x = -2;
  if (position_.x < left_x + 1) {
    force += glm::vec3(-position_.x/(position_.x-left_x), 0, 0);
  }
  if (position_.x > right_x - 1) {
    force += glm::vec3(position_.x/(position_.x-right_x), 0, 0);
  }

  float close_z = 2;
  float far_z = -2;
  if (position_.z < far_z + 1) {
    force += glm::vec3(0, 0, -position_.z/(position_.z-far_z));
  }
  if (position_.z > close_z - 1) {
    force += glm::vec3(0, 0, position_.z/(position_.z-close_z));
  }
  return force;
}


glm::vec3 BoidNode::seek(const glm::vec3 desired)
{
  glm::vec3 dir = desired - position_;

  // TODO: maximum
  return dir;
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
