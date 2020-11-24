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
  BoidNode(const std::string& filename);
  void Update(double delta_time) override;

 private:
  void LoadMeshFile(const std::string& filename);
  void UpdateNormals();

  std::shared_ptr<ShaderProgram> shader_;
  std::shared_ptr<VertexObject> mesh_;
  SceneNode* mesh_node_;

  PositionArray mesh_positions_;
};
}  // namespace GLOO

#endif
