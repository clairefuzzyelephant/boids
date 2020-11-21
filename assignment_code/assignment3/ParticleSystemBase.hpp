#ifndef PARTICLE_SYSTEM_BASE_H_
#define PARTICLE_SYSTEM_BASE_H_

#include "ParticleState.hpp"
#include "SpringInfo.hpp"

namespace GLOO {
class ParticleSystemBase {
 public:
  virtual ~ParticleSystemBase() {
  }
  
  std::vector<SpringInfo> spring_pairs_;
  mutable bool wind_on_ = false;
  
  virtual void ToggleWind() const {
  }

  virtual ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const = 0;
};
}  // namespace GLOO

#endif
