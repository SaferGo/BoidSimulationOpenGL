#include <boidSimulation/util.h>

#include <boidSimulation/config.h>

glm::vec2 util::generateRandVelocity()
{
   std::uniform_real_distribution<> randD(0.0, 1.0);
   std::uniform_real_distribution<> randS(
         config::MIN_SPEED, 
         config::MAX_SPEED
   );

   glm::vec2 direction = glm::normalize(
         glm::vec2(
            (randD(config::gen) * 2.0) - 1.0,
            (randD(config::gen) * 2.0) - 1.0
         )
   );
   float speed = randS(config::gen);

   return direction * speed;
}

glm::vec2 util::setMag(const glm::vec2 v, const float mag)
{
   return glm::normalize(v) * mag;
}

glm::vec2 util::clampMag(const glm::vec2 v, const float mag)
{
   if (glm::length(v) > mag)
      return glm::normalize(v) * mag;
   else
      return v;
}

// Calculates the angle between the direction vectors.
// To do that, first it has to clamp the dot product between [-1.0, 1.0] 
// because if not, we gonna receive NaN from glm::acos.
float util::getAngle(const glm::vec2 a, const glm::vec2 b)
{
   glm::vec2 d1 = glm::normalize(a);
   glm::vec2 d2 = glm::normalize(b);

   return glm::clamp(glm::dot(d1, d2), -1.0f, 1.0f);
}

bool util::isZeroVector(const glm::vec2 v)
{
   return (v.x == 0.0 && v.y == 0.0);
}

glm::vec2 util::getSteeringVector(const glm::vec2 desired, const glm::vec2 v)
{
   // Limits the maximum steering force
   return clampMag(desired - v, config::MAX_MAG);
}
