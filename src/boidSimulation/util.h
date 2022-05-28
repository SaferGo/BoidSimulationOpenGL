#pragma once

#include <glm/glm.hpp>

namespace util
{
   glm::vec2 generateRandVelocity();
   glm::vec2 setMag(const glm::vec2 v, const float mag);
   glm::vec2 clampMag(const glm::vec2 v, const float mag);
   glm::vec2 getSteeringVector(const glm::vec2 desired, const glm::vec2 v);
   float getAngle(const glm::vec2 a, const glm::vec2 b);
   bool isZeroVector(const glm::vec2 v);
}
