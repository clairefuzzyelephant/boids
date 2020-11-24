#include "BoidNode.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/shaders/PhongShader.hpp"

namespace GLOO {
BoidNode::BoidNode(const std::string& filename) : SceneNode() {
  LoadMeshFile(filename);
  shader_ = std::make_shared<PhongShader>();
}

void BoidNode::Update(double delta_time) {

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
