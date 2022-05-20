#ifndef UTIL_H
#define UTIL_H

#include <glm/glm.hpp>
#include <boidSimulation/Obstacle.h>

namespace util
{
   glm::vec2 generateRandVelocity();
   glm::vec2 setMag(const glm::vec2 v, const float mag);
   glm::vec2 clampMag(const glm::vec2 v, const float mag);
   glm::vec2 getSteeringVector(const glm::vec2 desired, const glm::vec2 v);
   float getAngle(const glm::vec2 a, const glm::vec2 b);
   bool isZeroVector(const glm::vec2 v);
   bool doesCollide(
         Obstacle& newObstacle, std::vector<Obstacle>& obstacles);
}

#endif
