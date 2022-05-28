#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <boidSimulation/Boid.h>
#include <boidSimulation/Obstacle.h>

class Flock
{

public:

   Flock();
   ~Flock();
   void toFlock(const std::vector<Obstacle>& obstacles);
   glm::vec2* getBoidPosition(const int i);
   glm::vec3* getColor();

private:

   std::vector<Boid> _boids;
   glm::vec3 _color[3];

   glm::vec2 alignment(const Boid& boid) const;
   glm::vec2 cohesion(const Boid& boid) const;
   glm::vec2 separation(const Boid& boid) const;
   
   glm::vec2 getAverageVector(const Boid& boid, const int type) const;
   glm::vec3 pickRandColor() const;
   glm::vec2 avoidObstacles(
         const Boid& boid, const std::vector<Obstacle>& obstacles
   ) const;
};
