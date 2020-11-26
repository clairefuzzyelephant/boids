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
  BoidNode(const std::string& filename, const glm::vec3 position);
  void UpdateBoids(double delta_time);
  void Run(const std::vector <BoidNode>& boids);
  
  glm::vec3 position_;
  glm::vec3 velocity_;
  glm::vec3 acceleration_;

 private:
  void LoadMeshFile(const std::string& filename);
  void AddForce(glm::vec3 force);
  
  void Flock(const std::vector<BoidNode>& boids);
  glm::vec3 Separation(const std::vector<BoidNode>& boids);
  glm::vec3 Alignment(const std::vector<BoidNode>& boids);
  glm::vec3 Cohesion(const std::vector<BoidNode>& boids);
  glm::vec3 seek(const glm::vec3 v);
  
  std::shared_ptr<ShaderProgram> shader_;
  std::shared_ptr<VertexObject> mesh_;
  SceneNode* mesh_node_;

  PositionArray mesh_positions_;
  float max_speed_;
  float max_force_;
};
}  // namespace GLOO

#endif
