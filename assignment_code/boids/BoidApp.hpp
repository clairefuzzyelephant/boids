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

// TODO: GUI for adjusting parameters?
//  protected:
//   void DrawGUI() override;

 private:
  BoidNode* boid_node_ptr_;
  std::unique_ptr<Flock> flock_;
};
}  // namespace GLOO

#endif
