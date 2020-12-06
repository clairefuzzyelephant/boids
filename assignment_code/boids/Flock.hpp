//
//  Flock.hpp
//  
//
//

#ifndef Flock_hpp
#define Flock_hpp

#include <iostream>
#include <vector>
#include "BoidNode.hpp"
#include "gloo/SceneNode.hpp"

namespace GLOO {
class Flock : public SceneNode {
public:
  Flock();
  int getSize();
  BoidNode* getBoid(int i);
  void addBoid(BoidNode* b);
  void addAttractiveObject(SceneNode* s);
  void setObjectPosition(glm::vec3 pos);
  void toggleObjectPositionView(bool viewable);
  void flocking();
  void Update(double delta_time) override;
  
  std::vector<BoidNode*> flock;
  std::vector<SceneNode*> attractive_objects;
private:
  glm::vec3 prev_camera_pos_;
  glm::quat prev_camera_rot_;

  glm::vec3 object_position_;
};

}

#endif /* Flock_hpp */
