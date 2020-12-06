#include "BoidApp.hpp"
#include <iostream>

#include "gloo/external.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/debug/AxisNode.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/shaders/PhongShader.hpp"
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

  auto camera_light = std::make_shared<DirectionalLight>();
  camera_light->SetDiffuseColor(glm::vec3(0.6f, 0.6f, 0.6f));
  camera_light->SetSpecularColor(glm::vec3(0.6f, 0.6f, 0.6f));
  camera_light->SetDirection(glm::vec3(0, 0, -1.0f));
  auto camera_light_node = make_unique<SceneNode>();
  camera_light_node->CreateComponent<LightComponent>(camera_light);
  root.AddChild(std::move(camera_light_node));

  AddBorders(root);

  auto flock = make_unique<Flock>();
  flock_ = flock.get();
  
  root.AddChild(std::move(flock));
}

void BoidApp::DrawGUI() {
  ImGui::Begin("Control Panel");
  ImGui::Text("Forces");
  ImGui::PushID(0);
  ImGui::SliderFloat("wall", &wall_force_slider_, 0, 100);
  ImGui::PopID();
  ImGui::PushID(1);
  ImGui::SliderFloat("separation", &separation_force_slider_, 0, 10);
  ImGui::PopID();
  ImGui::PushID(2);
  ImGui::SliderFloat("alignment", &alignment_force_slider_, 0, 10);
  ImGui::PopID();
  ImGui::PushID(3);
  ImGui::SliderFloat("cohesion", &cohesion_force_slider_, 0, 10);
  ImGui::PopID();

  ImGui::Text("Predator");
  ImGui::PushID(4);
  ImGui::SliderFloat("speed", &predator_speed_slider_, 0.8, 1);
  ImGui::PopID();

  ImGui::Text("Add Attractive Object");
  ImGui::PushID(5);
  ImGui::Checkbox("enabled", &placing);
  ImGui::PushID(6);
  ImGui::SliderFloat("x", &x, -3, 3);
  ImGui::PopID();
  ImGui::PushID(7);
  ImGui::SliderFloat("y", &y, -3, 3);
  ImGui::PopID();
  ImGui::PushID(8);
  ImGui::SliderFloat("z", &z, -3, 3);
  ImGui::PopID();
  ImGui::End();

  for (auto& boid : flock_->flock) {
    boid->SetForceCoefficients(wall_force_slider_, predator_speed_slider_, separation_force_slider_, alignment_force_slider_, cohesion_force_slider_);
  }

  flock_->setObjectPosition(glm::vec3(x, y, z));
  flock_->toggleObjectPositionView(placing);
}

void BoidApp::AddBorders(SceneNode& root) {
  auto shader = std::make_shared<PhongShader>();
  auto top_bottom_color = glm::vec3(255, 255, 255)/255.f;

  auto floor = make_unique<VertexObject>();
  float fw = 5; // floor width
  float h = 3;  // height
  auto positions = make_unique<PositionArray>();
  positions->push_back(glm::vec3(fw, -h, fw));
  positions->push_back(glm::vec3(-fw, -h, -fw));
  positions->push_back(glm::vec3(fw, -h, -fw));
  positions->push_back(glm::vec3(-fw, -h, fw));
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
  mat.GetMaterial().SetDiffuseColor(top_bottom_color);
  root.AddChild(std::move(floor_node));
  
  auto ceiling = make_unique<VertexObject>();
  positions = make_unique<PositionArray>();
  positions->push_back(glm::vec3(fw, h, fw));
  positions->push_back(glm::vec3(-fw, h, -fw));
  positions->push_back(glm::vec3(fw, h, -fw));
  positions->push_back(glm::vec3(-fw, h, fw));
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
  for (int i = 0; i < 6; i++)
    normals->push_back(glm::vec3(0, 1, 0));
  ceiling->UpdateNormals(std::move(normals));

  auto ceiling_node = make_unique<SceneNode>();
  ceiling_node->CreateComponent<ShadingComponent>(shader);
  ceiling_node->CreateComponent<RenderingComponent>(std::move(ceiling));
  mat = ceiling_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  mat.GetMaterial().SetDiffuseColor(top_bottom_color);
  root.AddChild(std::move(ceiling_node));

  auto right_left_color = glm::vec3(100.f, 100.f, 100.f)/255.f;

  auto right_wall = make_unique<VertexObject>();
  positions = make_unique<PositionArray>();
  positions->push_back(glm::vec3(fw, h, -fw));
  positions->push_back(glm::vec3(fw, -h, fw));
  positions->push_back(glm::vec3(fw, h, fw));
  positions->push_back(glm::vec3(fw, -h, -fw));
  right_wall->UpdatePositions(std::move(positions));
  indices = make_unique<IndexArray>();
  indices->push_back(0);
  indices->push_back(1);
  indices->push_back(2);
  indices->push_back(0);
  indices->push_back(1);
  indices->push_back(3);
  right_wall->UpdateIndices(std::move(indices));
  normals = make_unique<NormalArray>();
  for (int i = 0; i < 6; i++)
    normals->push_back(glm::vec3(1, 0, 0));
  right_wall->UpdateNormals(std::move(normals));

  auto right_wall_node = make_unique<SceneNode>();
  right_wall_node->CreateComponent<ShadingComponent>(shader);
  right_wall_node->CreateComponent<RenderingComponent>(std::move(right_wall));
  mat = right_wall_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  mat.GetMaterial().SetDiffuseColor(right_left_color);
  root.AddChild(std::move(right_wall_node));

  auto left_wall = make_unique<VertexObject>();
  positions = make_unique<PositionArray>();
  positions->push_back(glm::vec3(-fw, h, -fw));
  positions->push_back(glm::vec3(-fw, -h, fw));
  positions->push_back(glm::vec3(-fw, h, fw));
  positions->push_back(glm::vec3(-fw, -h, -fw));
  left_wall->UpdatePositions(std::move(positions));
  indices = make_unique<IndexArray>();
  indices->push_back(0);
  indices->push_back(1);
  indices->push_back(2);
  indices->push_back(0);
  indices->push_back(1);
  indices->push_back(3);
  left_wall->UpdateIndices(std::move(indices));
  normals = make_unique<NormalArray>();
  for (int i = 0; i < 6; i++)
    normals->push_back(glm::vec3(1, 0, 0));
  left_wall->UpdateNormals(std::move(normals));

  auto left_wall_node = make_unique<SceneNode>();
  left_wall_node->CreateComponent<ShadingComponent>(shader);
  left_wall_node->CreateComponent<RenderingComponent>(std::move(left_wall));
  mat = left_wall_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  mat.GetMaterial().SetDiffuseColor(right_left_color);
  root.AddChild(std::move(left_wall_node));

  auto back_color = glm::vec3(50.f, 50.f, 50.f)/255.f;

  auto back_wall = make_unique<VertexObject>();
  positions = make_unique<PositionArray>();
  positions->push_back(glm::vec3(-fw, h, -fw));
  positions->push_back(glm::vec3(fw, -h, -fw));
  positions->push_back(glm::vec3(fw, h, -fw));
  positions->push_back(glm::vec3(-fw, -h, -fw));
  back_wall->UpdatePositions(std::move(positions));
  indices = make_unique<IndexArray>();
  indices->push_back(0);
  indices->push_back(1);
  indices->push_back(2);
  indices->push_back(0);
  indices->push_back(1);
  indices->push_back(3);
  back_wall->UpdateIndices(std::move(indices));
  normals = make_unique<NormalArray>();
  for (int i = 0; i < 6; i++)
    normals->push_back(glm::vec3(0, 0, 1));
  back_wall->UpdateNormals(std::move(normals));

  auto back_wall_node = make_unique<SceneNode>();
  back_wall_node->CreateComponent<ShadingComponent>(shader);
  back_wall_node->CreateComponent<RenderingComponent>(std::move(back_wall));
  mat = back_wall_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  mat.GetMaterial().SetDiffuseColor(back_color);
  root.AddChild(std::move(back_wall_node));
}

} // namespace GLOO
