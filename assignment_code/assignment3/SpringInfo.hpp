//
//  SpringInfo.hpp
//  
//
//  Created by Claire Cheng on 10/16/20.
//

#ifndef SPRING_INFO_H_
#define SPRING_INFO_H_

#include <stdio.h>

namespace GLOO {
struct SpringInfo {
  int parent_index; int child_index; float rest_distance; float stiffness;
};
}  // namespace GLOO

#endif /* SpringInfo_hpp */
