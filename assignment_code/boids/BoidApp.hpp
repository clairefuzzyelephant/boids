#ifndef BOID_APP_H_
#define BOID_APP_H_

#include "gloo/Application.hpp"

#include "BoidNode.hpp"

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
};
}  // namespace GLOO

#endif
