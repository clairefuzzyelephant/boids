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
  void setAttractiveObject(SceneNode* s);
  void setObjectPosition(glm::vec3 pos);
  void toggleObjectPositionView(bool viewable);
  void flocking();
  void Update(double delta_time) override;
  void checkAttractorExpired();
  
  std::vector<BoidNode*> flock;
  SceneNode* attractive_object_;
  bool is_attracting_;
private:
  glm::vec3 prev_camera_pos_;
  glm::quat prev_camera_rot_;

  glm::vec3 object_position_;
  int object_placed_; //index at which the attractor object is placed
};

}

#endif /* Flock_hpp */
