//
//  Flock.cpp
//  
//
//

#include "BoidNode.hpp"
#include "Flock.hpp"

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
  for (uint i = 0; i < flock.size(); i++)
    flock[i]->Run(flock, delta_time);
}

}
