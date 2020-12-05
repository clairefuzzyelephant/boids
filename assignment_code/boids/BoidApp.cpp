#include "BoidApp.hpp"
#include <iostream>

#include "gloo/external.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/debug/AxisNode.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"

#include "glm/gtx/string_cast.hpp"

namespace GLOO {
BoidApp::BoidApp(const std::string& app_name,
                 glm::ivec2 window_size
                ) : Application(app_name, window_size) {}

void BoidApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  // TODO: customer perspective moving camera?
  auto camera_node = make_unique<ArcBallCameraNode>(45.f, 0.75f, 3.0f);
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));

  root.AddChild(make_unique<AxisNode>('A'));

  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.1f));
  root.CreateComponent<LightComponent>(ambient_light);

  auto sun_light = std::make_shared<DirectionalLight>();
  sun_light->SetDiffuseColor(glm::vec3(0.6f, 0.6f, 0.6f));
  sun_light->SetSpecularColor(glm::vec3(0.6f, 0.6f, 0.6f));
  sun_light->SetDirection(glm::vec3(-1.0f, -0.8f, -1.0f));
  auto sun_light_node = make_unique<SceneNode>();
  sun_light_node->CreateComponent<LightComponent>(sun_light);
  root.AddChild(std::move(sun_light_node));

  AddBorders(root);

  flock_ = make_unique<Flock>();
  
  root.AddChild(std::move(flock_));
}

void BoidApp::AddBorders(SceneNode& root) {
  auto shader = std::make_shared<SimpleShader>();
  auto color = glm::vec3(50, 50, 50)/255.f;

  auto floor = make_unique<VertexObject>();
  float fw = 10; // floor width
  auto positions = make_unique<PositionArray>();
  positions->push_back(glm::vec3(fw, -2, fw));
  positions->push_back(glm::vec3(-fw, -2, -fw));
  positions->push_back(glm::vec3(fw, -2, -fw));
  positions->push_back(glm::vec3(-fw, -2, fw));
  floor->UpdatePositions(std::move(positions));
  auto indices = make_unique<IndexArray>();
  indices->push_back(0);
  indices->push_back(1);
  indices->push_back(2);
  indices->push_back(0);
  indices->push_back(1);
  indices->push_back(3);
  floor->UpdateIndices(std::move(indices));
  auto normals = make_unique<NormalArray>();
  normals->push_back(glm::vec3(0, 1, 0));
  normals->push_back(glm::vec3(0, 1, 0));
  floor->UpdateNormals(std::move(normals));

  auto floor_node = make_unique<SceneNode>();
  floor_node->CreateComponent<ShadingComponent>(shader);
  floor_node->CreateComponent<RenderingComponent>(std::move(floor));
  auto mat = floor_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  mat.GetMaterial().SetDiffuseColor(color);
  root.AddChild(std::move(floor_node));
  
  auto ceiling = make_unique<VertexObject>();
  positions = make_unique<PositionArray>();
  positions->push_back(glm::vec3(fw, 4, fw));
  positions->push_back(glm::vec3(-fw, 4, -fw));
  positions->push_back(glm::vec3(fw, 4, -fw));
  positions->push_back(glm::vec3(-fw, 4, fw));
  ceiling->UpdatePositions(std::move(positions));
  indices = make_unique<IndexArray>();
  indices->push_back(0);
  indices->push_back(1);
  indices->push_back(2);
  indices->push_back(0);
  indices->push_back(1);
  indices->push_back(3);
  ceiling->UpdateIndices(std::move(indices));
  normals = make_unique<NormalArray>();
  normals->push_back(glm::vec3(0, 1, 0));
  normals->push_back(glm::vec3(0, 1, 0));
  ceiling->UpdateNormals(std::move(normals));

  auto ceiling_node = make_unique<SceneNode>();
  ceiling_node->CreateComponent<ShadingComponent>(shader);
  ceiling_node->CreateComponent<RenderingComponent>(std::move(ceiling));
  mat = ceiling_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  mat.GetMaterial().SetDiffuseColor(color);
  root.AddChild(std::move(ceiling_node));

}
} // namespace GLOO
