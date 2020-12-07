//
//  Flock.cpp
//  
//
//

#include "BoidNode.hpp"
#include "Flock.hpp"

#include "gloo/InputManager.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/SceneNode.hpp"

#include "glm/gtx/string_cast.hpp"

namespace GLOO {

Flock::Flock() {
  auto sphere = make_unique<SceneNode>();
  std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
  sphere->CreateComponent<ShadingComponent>(shader);
  std::shared_ptr<VertexObject> mesh = PrimitiveFactory::CreateSphere(0.05f, 25, 25);
  sphere->CreateComponent<RenderingComponent>(mesh);
  sphere->GetTransform().SetPosition(object_position_);
  auto mat = sphere->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  mat.GetMaterial().SetDiffuseColor(glm::vec3(50.f/255.f));
  sphere->SetActive(false);
  AddChild(std::move(sphere));

  attractive_object_ = new SceneNode();
  is_attracting_ = false;
  object_position_ = glm::vec3(0.f, 0.f, 0.f);
  object_placed_ = -1; // has not been placed

  for (int i = 0; i < 25; i++) {
    auto boid = make_unique<BoidNode>("pierog.obj", glm::vec3(rand()%5 * 0.2f, rand()%5 * 0.2f, rand()%5 * 0.2f), false, i);
    addBoid(boid.get());
    AddChild(std::move(boid));
  }
}

int Flock::getSize()
{
    return flock.size();
}

BoidNode* Flock::getBoid(int i)
{
    return flock[i];
}

void Flock::addBoid(BoidNode* b)
{
  flock.push_back(b);
}

void Flock::setAttractiveObject(SceneNode* s) {
  attractive_object_ = s;
  is_attracting_ = true;
}

void Flock::setObjectPosition(glm::vec3 pos) {
  object_position_ = pos;
  GetChild(0).GetTransform().SetPosition(pos);
}

void Flock::toggleObjectPositionView(bool viewable) {
  GetChild(0).SetActive(viewable);
}

// Runs the run function for every boid in the flock checking against the flock
// itself. Which in turn applies all the rules to the flock.
void Flock::flocking() {}

void Flock::checkAttractorExpired() {
  if (object_placed_ != -1 and is_attracting_) {
    for (auto boid : flock) {
      if (std::abs(boid->position_.x - object_position_.x) > 0.2f) {
        continue;
      }
      if (std::abs(boid->position_.y - object_position_.y) > 0.2f) {
        continue;
      }
      if (std::abs(boid->position_.z - object_position_.z) > 0.2f) {
        continue;
      }
      //remove attractor object if 'collision'
      GetChild(object_placed_).SetActive(false);
      is_attracting_ = false;
    }
  }
  
}

void Flock::Update(double delta_time) {
  static bool paused = false;
  checkAttractorExpired();

  auto ave_pos = glm::vec3(0);
  auto ave_vel = glm::vec3(0);
  for (uint i = 0; i < flock.size(); i++) {
    if (!paused)
      flock[i]->Run(flock, attractive_object_, is_attracting_, delta_time);
    ave_pos += flock[i]->position_;
    ave_vel += flock[i]->velocity_;
  }
  ave_pos /= flock.size();

  auto& camera_node = GetParentPtr()->GetChild(0);
  ArcBallCameraNode& cam = static_cast<ArcBallCameraNode&>(camera_node);

  static bool prev_released = true;
  static bool pierogi_pov = false;
  // toggle to a POV of a pierogi on pressing c
  if (InputManager::GetInstance().IsKeyPressed('C')) {
    if (prev_released) {
      pierogi_pov = !pierogi_pov;
      // if switching to pierogi view, save prev position
      if (pierogi_pov) {
        prev_camera_pos_ = camera_node.GetTransform().GetPosition();
        prev_camera_rot_ = camera_node.GetTransform().GetRotation();
      } else {
        // if switching to regular view, restore prev position
        camera_node.GetTransform().SetPosition(prev_camera_pos_);
        camera_node.GetTransform().SetRotation(prev_camera_rot_);
      }
      cam.TogglePOV();
    }
    prev_released = false;
  } else if (InputManager::GetInstance().IsKeyPressed('P')) {
    if (prev_released) {
      glm::vec3 pos = glm::vec3(rand()%5 * 0.2f, rand()%5 * 0.2f, rand()%5 * 0.2f);
      auto predator = make_unique<BoidNode>("another_fork.obj", pos, true, 0);
      addBoid(predator.get());
      AddChild(std::move(predator));
      std::cout << "added predator at " << glm::to_string(pos) << std::endl;
    }
    prev_released = false;
  } else if (InputManager::GetInstance().IsKeyPressed('D')) {
    if (prev_released) {
      if (object_placed_ == -1) {
        auto sphere = make_unique<SceneNode>();
        std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
        sphere->CreateComponent<ShadingComponent>(shader);
        std::shared_ptr<VertexObject> mesh = PrimitiveFactory::CreateSphere(0.05f, 25, 25);
        sphere->CreateComponent<RenderingComponent>(mesh);
        sphere->GetTransform().SetPosition(object_position_);
        sphere->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
        setAttractiveObject(sphere.get());
        AddChild(std::move(sphere));
        object_placed_ = GetChildrenCount() - 1;
      }
      else {
        GetChild(object_placed_).GetTransform().SetPosition(object_position_);
        GetChild(object_placed_).SetActive(true);
        is_attracting_ = true;
      }
      
      std::cout << "added attractive object at " << glm::to_string(object_position_) << std::endl;
    }
    prev_released = false;
  } else if (InputManager::GetInstance().IsKeyPressed(' ')) {
    if (prev_released) {
      paused = !paused;
    }
    prev_released = false;
  } else
  {
    prev_released = true;
  }

  if (pierogi_pov) {
    cam.GetTransform().SetPosition(ave_pos - 1.0f*glm::normalize(ave_vel));
    cam.SetDirection(glm::normalize(ave_vel));
  }
}

}
