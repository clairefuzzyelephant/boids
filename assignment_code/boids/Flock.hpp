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

namespace GLOO {
class Flock {
public:
  Flock();
  int getSize();
  BoidNode* getBoid(int i);
  void addBoid(BoidNode* b);
  void flocking();
  
  std::vector<BoidNode*> flock;
};

}

#endif /* Flock_hpp */
