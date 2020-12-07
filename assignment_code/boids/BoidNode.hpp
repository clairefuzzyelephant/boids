#ifndef BOID_NODE_H_
#define BOID_NODE_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"

#include <string>
#include <vector>

namespace GLOO {
class BoidNode : public SceneNode {
 public:
  BoidNode(const std::string& filename, const glm::vec3 position, const bool is_predator, int boid_id);
  void UpdateBoid(double delta_time);
  void Run(const std::vector<BoidNode*>& boids,  SceneNode* attractive_object, bool object_active, double delta_time);
  // sets values from sliders
  void SetForceCoefficients(float wall_force, float predator_speed, float separation_force, float alignment_force, float cohesion_force, float attraction_force) {
    wall_force_ = wall_force;
    predator_speed_ = predator_speed;
    separation_coeff_ = separation_force;
    alignment_coeff_ = alignment_force;
    cohesion_coeff_ = cohesion_force;
    attractor_coeff_ = attraction_force;
  }
  
  glm::vec3 position_;
  glm::vec3 velocity_;
  glm::vec3 acceleration_;
  bool is_predator_;
  int id_;

 private:
  void LoadMeshFile(const std::string& filename);
  void AddForce(glm::vec3 force);
  
  void Flock(const std::vector<BoidNode*>& boids, SceneNode* attractive_object, bool object_active, double delta_time);
  glm::vec3 Separation(const std::vector<BoidNode*>& boids);
  glm::vec3 Alignment(const std::vector<BoidNode*>& boids);
  glm::vec3 Cohesion(const std::vector<BoidNode*>& boids);
  glm::vec3 Avoidance();
  glm::vec3 Attraction(const std::vector<BoidNode*>& boids, const SceneNode* attractive_object);
  glm::vec3 seek(const glm::vec3 v);
  float ForceCurve(float x, float border);
  
  std::shared_ptr<ShaderProgram> shader_;
  std::shared_ptr<VertexObject> mesh_;
  SceneNode* mesh_node_;

  PositionArray mesh_positions_;
  float max_speed_;
  float max_force_;

  float wall_force_;
  float predator_speed_;
  float separation_coeff_;
  float alignment_coeff_;
  float cohesion_coeff_;
  float attractor_coeff_;
};
}  // namespace GLOO

#endif
