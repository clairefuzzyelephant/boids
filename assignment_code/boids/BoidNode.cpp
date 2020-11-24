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

  UpdateNormals();
}

void BoidNode::UpdateNormals() {
  PositionArray positions = mesh_->GetPositions();
  IndexArray indices = mesh_->GetIndices();
  std::map<unsigned int, std::vector<glm::vec3>> normal_map;
  for (uint i = 0; i < indices.size(); i+= 3) {
    glm::vec3 a = positions[indices[i]];
    glm::vec3 b = positions[indices[i+1]];
    glm::vec3 c = positions[indices[i+2]];
    glm::vec3 ac = c - a;
    glm::vec3 ab = b - a;

    glm::vec3 normal = glm::cross(ab, ac);
    normal_map[indices[i]].push_back(normal);
    normal_map[indices[i+1]].push_back(normal);
    normal_map[indices[i+2]].push_back(normal);
  }

  auto normals = make_unique<NormalArray>();
  for (uint i = 0; i < normal_map.size(); i++) {
    auto normal = glm::vec3(0);
    for (auto& adjacent_normal : normal_map[i]) {
      normal += adjacent_normal;
    }
    normals->push_back(glm::normalize(normal));
  }
  mesh_->UpdateNormals(std::move(normals));
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
