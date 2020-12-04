#include "BoidApp.hpp"
#include <iostream>

#include "gloo/external.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/components/LightComponent.hpp"

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
  
  flock_ = make_unique<Flock>();
  
  srand(3);
  for (int i = 0; i < 20; i++) {
    auto boid = make_unique<BoidNode>("pierog.obj", glm::vec3(rand()%5 * 0.2f, rand()%5 * 0.2f, rand()%5 * 0.2f));
    flock_->addBoid(boid.get());
    root.AddChild(std::move(boid));
  }
  root.AddChild(std::move(flock_));
}
} // namespace GLOO
