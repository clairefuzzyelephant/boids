//
//  Flock.cpp
//  
//
//

#include "BoidNode.hpp"
#include "Flock.hpp"

#include "gloo/InputManager.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"

namespace GLOO {

Flock::Flock() {
  for (int i = 0; i < 25; i++) {
    auto boid = make_unique<BoidNode>("pierog.obj", glm::vec3(rand()%5 * 0.2f, rand()%5 * 0.2f, rand()%5 * 0.2f));
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

// Runs the run function for every boid in the flock checking against the flock
// itself. Which in turn applies all the rules to the flock.
void Flock::flocking() {}

void Flock::Update(double delta_time) {
  auto ave_pos = glm::vec3(0);
  auto ave_vel = glm::vec3(0);
  for (uint i = 0; i < flock.size(); i++) {
    flock[i]->Run(flock, delta_time);
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
  } else {
    prev_released = true;
  }

  if (pierogi_pov) {
    cam.GetTransform().SetPosition(ave_pos - 1.0f*glm::normalize(ave_vel));
    cam.SetDirection(glm::normalize(ave_vel));
  }
}

}
