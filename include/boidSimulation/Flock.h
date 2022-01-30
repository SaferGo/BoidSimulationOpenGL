#ifndef FLOCK_H
#define FLOCK_H

#include <vector>

#include <glm/glm.hpp>

#include <boidSimulation/Boid.h>

class Flock
{

public:
   std::vector<Boid> boids;

   Flock(int n);
   void toFlock();

private:

   glm::vec2 alignment(const Boid& boid);
   glm::vec2 cohesion(const Boid& boid);
   glm::vec2 separation(const Boid& boid);
   
   glm::vec2 getAverageVector(const Boid& boid, const int type);
   int getSize();
};

#endif
