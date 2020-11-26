//
//  Flock.cpp
//  
//
//

#include "BoidNode.hpp"
#include "Flock.hpp"

namespace GLOO {

int Flock::getSize()
{
    return flock.size();
}

BoidNode Flock::getBoid(int i)
{
    return flock[i];
}

void Flock::addBoid(BoidNode* b)
{
  flock.push_back(*b);
}

// Runs the run function for every boid in the flock checking against the flock
// itself. Which in turn applies all the rules to the flock.
void Flock::flocking()
{
  for (int i = 0; i < flock.size(); i++)
    flock[i].Run(flock);
}

}

