#include "BoidNode.hpp"

#include <map>
#include "gloo/MeshLoader.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"

namespace GLOO {
BoidNode::BoidNode(const std::string& filename) : SceneNode() {
  LoadMeshFile(filename);
  shader_ = std::make_shared<PhongShader>();

  auto mesh_node = make_unique<SceneNode>();
  mesh_node->CreateComponent<ShadingComponent>(shader_);
  mesh_node->CreateComponent<RenderingComponent>(mesh_);
  mesh_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  mesh_node->SetActive(true);
  mesh_node_ = mesh_node.get();
  AddChild(std::move(mesh_node));
  mesh_node_->GetTransform().SetScale(glm::vec3(0.01));
}

void BoidNode::Update(double delta_time) {
  // TODO: implement rules and call them here
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
