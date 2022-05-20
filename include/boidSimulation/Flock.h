#ifndef FLOCK_H
#define FLOCK_H

#include <vector>

#include <glm/glm.hpp>

#include <boidSimulation/Boid.h>
#include <boidSimulation/Obstacle.h>

class Flock
{

public:
   std::vector<Boid> boids;
   glm::vec3 color[3];

   Flock(const int n);
   void toFlock(const std::vector<Obstacle> &obstacles);

private:

   glm::vec2 alignment(const Boid& boid);
   glm::vec2 cohesion(const Boid& boid);
   glm::vec2 separation(const Boid& boid);
   
   glm::vec2 getAverageVector(const Boid& boid, const int type);
   glm::vec3 pickRandColor();
   int getSize();
   glm::vec2 avoidObstacles(
         const Boid& boid, const std::vector<Obstacle> &obstacles);
};

#endif
