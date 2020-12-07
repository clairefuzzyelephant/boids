#ifndef BOID_APP_H_
#define BOID_APP_H_

#include "gloo/Application.hpp"

#include "BoidNode.hpp"
#include "Flock.hpp"

namespace GLOO {
class BoidApp : public Application {
 public:
  BoidApp(const std::string& app_name,
          glm::ivec2 window_size);
  void SetupScene() override;
  void Update(double delta_time);

 protected:
  void DrawGUI() override;

 private:
  void AddBorders(SceneNode& root);
  Flock* flock_;
  float wall_force_slider_ = 35;
  float predator_speed_slider_ = 0.98;
  float separation_force_slider_ = 5;
  float alignment_force_slider_ = 5;
  float cohesion_force_slider_ = 5;
  float attractor_force_slider_ = 5;

  bool placing = false;
  float x = 0.f;
  float y = 0.f;
  float z = 0.f;
};
}  // namespace GLOO

#endif
