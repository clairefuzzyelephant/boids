#include "BoidNode.hpp"

#include <map>
#include "gloo/MeshLoader.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"

#include "glm/gtx/string_cast.hpp"

namespace GLOO {
BoidNode::BoidNode(const std::string& filename, const glm::vec3 position, const bool is_predator, int boid_id) : SceneNode() {
  LoadMeshFile(filename);
  shader_ = std::make_shared<PhongShader>();
  auto offset = glm::vec3(-0.1, -0.03, -0.17);
  id_ = boid_id;

  auto mesh_node = make_unique<SceneNode>();
  auto inner_node = make_unique<SceneNode>();
  inner_node->CreateComponent<ShadingComponent>(shader_);
  inner_node->CreateComponent<RenderingComponent>(mesh_);
  auto mat = inner_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  if (!is_predator) {
    mat.GetMaterial().SetDiffuseColor(glm::vec3(245.f, 233.f, 154.f)/255.f);
    // Inner node necessary to fix .obj being slightly off center. Offset here fixes.
    inner_node->GetTransform().SetPosition(offset/6.25f);
    // If changing scale, may need to change offset proportionally
    inner_node->GetTransform().SetScale(glm::vec3(0.005));
  }
  else {
    mat.GetMaterial().SetDiffuseColor(glm::vec3(50.f, 50.f, 50.f)/255.f);
    inner_node->GetTransform().SetScale(glm::vec3(0.01));
    glm::quat rotation = glm::quat(glm::vec3(0, 180, 0));
    inner_node->GetTransform().SetRotation(rotation);
  }
  mesh_node->AddChild(std::move(inner_node));
  mesh_node_ = mesh_node.get();
  AddChild(std::move(mesh_node));
  mesh_node_->GetTransform().SetPosition(position);

  position_ = position;
  velocity_ = glm::vec3(rand()%10 * 0.1f, rand()%10 * 0.1f, rand()%10 * 0.1f);
  acceleration_ = glm::vec3(0);
  
  max_speed_ = 5.f;
  max_force_ = 0.1f;
  is_predator_ = is_predator;

}

void BoidNode::UpdateBoid(double delta_time) {
  acceleration_ = acceleration_ * 0.5f; // dampen
  velocity_ = velocity_ + (acceleration_ * (float)delta_time);
  if (is_predator_) {
    velocity_ *= predator_speed_;
  }

  if (glm::length(velocity_) > max_speed_) {
    velocity_ = glm::normalize(velocity_) * 4.f;
  }
  // std::cout << "velocity" << glm::to_string(velocity_) << std::endl;
  position_ = position_ + (velocity_ * (float)delta_time);
  // float sides = 4.8;
  // float height = 4.8;
  // if (std::abs(position_.x) > sides) {
  //   velocity_.x = -velocity_.x * 0.6;
  // }
  // if (std::abs(position_.y) > height) {
  //   velocity_.y = -velocity_.y * 0.6;
  //   if (position_.y > height) {
  //     position_.y = height;
  //   }
  //   else if (position_.y < -height) {
  //     position_.y = -height;
  //   }
  // }
  // if (std::abs(position_.z) > sides) {
  //   velocity_.z = -velocity_.z * 0.6;
  // }
  mesh_node_->GetTransform().SetPosition(position_);

  glm::quat rot;
  if (is_predator_) {
    rot = glm::quat(glm::vec3(0, 1, 0)) * glm::quatLookAt(glm::normalize(velocity_), glm::vec3(0, 1, 0));
  } else {
    rot = glm::quat(glm::vec3(0, -3.14/2.f, 0)) * glm::quatLookAt(glm::normalize(velocity_), glm::vec3(0, 1, 0));
  }
  mesh_node_->GetTransform().SetRotation(rot);

  acceleration_ = glm::vec3(0.f, 0.f, 0.f);
  
}

void BoidNode::Run(const std::vector<BoidNode*>& boids,  SceneNode* attractive_object, bool object_active, double delta_time)
{
    Flock(boids, attractive_object, object_active, delta_time);
    UpdateBoid(delta_time);
    // std::cout << "position" << glm::to_string(position_) << std::endl;
}

void BoidNode::Flock(const std::vector<BoidNode*>& boids, SceneNode* attractive_object, bool object_active, double delta_time) {
  glm::vec3 separation = Separation(boids);
  glm::vec3 alignment = Alignment(boids);
  glm::vec3 cohesion = Cohesion(boids);
  glm::vec3 avoidance = Avoidance();
  glm::vec3 attraction = glm::vec3(0.f, 0.f, 0.f);
  if (object_active) {
    attraction = Attraction(boids, attractive_object);
  }
  
  AddForce(separation * float(delta_time) * 60.f * separation_coeff_);
  AddForce(alignment * float(delta_time) * 20.f * alignment_coeff_);
  AddForce(cohesion * float(delta_time) * 60.f * cohesion_coeff_);
  AddForce(avoidance * (float)delta_time * 500.f * wall_force_);
  AddForce(attraction * float(delta_time) * 60.f * attractor_coeff_);
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
  glm::vec3 predator_force(0.f, 0.f, 0.f);
  // For every boid in the system, check if it's too close
  for (uint i = 0; i < boids.size(); i++) {
      if (i == id_) {
        continue;
      }
      // Calculate distance from current boid to boid we're looking at
      float d = glm::distance(position_, boids[i]->position_);
      // If this is a fellow boid and it's too close, move away from it
      if ((d >= 0) && (d < desiredseparation) && !is_predator_ && !boids[i]->is_predator_) {
        glm::vec3 diff;
        if (d == 0) {
          diff = glm::vec3(rand()%3, rand()%3, rand()%3);
          diff = glm::normalize(diff);
        }
        else {
          diff = position_ -  boids[i]->position_;
          if (glm::length(diff) > 0) {
            diff = glm::normalize(diff);
          }
          diff = diff/d;
        }
        steer = steer + diff;
        count++;
      }
      //both predators
      else if ((d > 0) && (d < desiredseparation) && is_predator_ && boids[i]->is_predator_) {
        // std::cout << "both predators" << std::endl;
        glm::vec3 diff = position_ -  boids[i]->position_;
        if (glm::length(diff) > 0) {
          diff = glm::normalize(diff);
        }
        diff = diff/d;
        diff = diff * 0.5f; //decrease distance
        steer = steer + diff;
        count++;
      }
      //running from predator
      else if ((d > 0) && (d < 2.f) && !is_predator_ && boids[i]->is_predator_) {
        // std::cout << "predator alert!!" << std::endl;
        glm::vec3 diff = position_ -  boids[i]->position_;
        if (glm::length(diff) > 0) {
          diff = glm::normalize(diff);
        }
        diff = diff/d;
        diff = diff * 800.f; //increase distance
        // std::cout << glm::to_string(diff) << std::endl;
        predator_force = predator_force + diff;
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
      steer = steer + predator_force;
  }
  return steer;
}

glm::vec3 BoidNode::Alignment(const std::vector<BoidNode*>& boids)
{
  float neighbordist = 50.f; // Field of vision

  glm::vec3 sum(0.f, 0.f, 0.f);
  int count = 0;
  for (uint i = 0; i < boids.size(); i++) {
    if (!boids[i]->is_predator_) {
      float d = glm::distance(position_, boids[i]->position_);
      if ((d > 0) && (d < neighbordist)) { // 0 < d < 50
          sum = sum + boids[i]->velocity_;
          count++;
      }
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
    if (!boids[i]->is_predator_) {
      float d = glm::distance(position_, boids[i]->position_);
      if ((d > 0) && (d < neighbordist)) {
          sum = sum + boids[i]->position_;
          count++;
      }
    }
  }
  if (count > 0) {
    sum = sum / float(count);
    if (is_predator_) {
      return seek(sum) * 0.75f;
    }
    else {
      return seek(sum);
    }
  } else {
    return glm::vec3(0);
  }
}

glm::vec3 BoidNode::Avoidance()
{
  glm::vec3 force = glm::vec3(0);
  float offset = 0.5; // offset from border that we apply force starting at

  // side walls
  force += glm::vec3(ForceCurve(position_.x, -4+offset), 0, 0);
  force += glm::vec3(ForceCurve(position_.x, 4-offset), 0, 0);

  // ceiling/floor
  force += glm::vec3(0, ForceCurve(position_.y, -2+offset), 0);
  force += glm::vec3(0, ForceCurve(position_.y, 2-offset), 0);

  // front/back
  force += glm::vec3(0, 0, ForceCurve(position_.z, -4+offset));
  force += glm::vec3(0, 0, ForceCurve(position_.z, 4-offset));


  // spherical force field
  float border = 4;
  if (glm::length(position_) > border) {
    float mag = exp(glm::length(position_ - glm::normalize(position_)*border) - 1.f);
    force = -mag * glm::normalize(position_);
  } else {
    force = glm::vec3(0);
  }

  return force;
}


// Attraction
// Pulls boids towards attractive object
glm::vec3 BoidNode::Attraction(const std::vector<BoidNode*>& boids, const SceneNode* attractive_object)
{
  if (!is_predator_) {
    return seek(attractive_object->GetTransform().GetPosition());
  }
  else {
    return glm::vec3(0);
  }
}

float BoidNode::ForceCurve(float x, float border) {
  float coeff = wall_force_;
  if (border > 0) {
    float f = exp(x-border) - 1;
    if (f > 0) return -f*coeff;
  } else {
    float f = exp(-(x-border)) - 1;
    if (f > 0) return f*coeff;
  }
  return 0;
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
