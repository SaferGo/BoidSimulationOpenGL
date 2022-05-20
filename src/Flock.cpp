#include <boidSimulation/Flock.h>

#include <boidSimulation/config.h>
#include <boidSimulation/util.h>

Flock::Flock(const int n)
{
   color[0] = color[1] = color[2] = pickRandColor();
   
   for (int i = 0; i < n; i++)
   {
      Boid newBoid = Boid(i);
      boids.push_back(newBoid);
   }
}

void Flock::toFlock(const std::vector<Obstacle> &obstacles)
{
   for (auto& boid: boids)
   {
      glm::vec2 newAcceleration = glm::vec2(0.0);
      
      newAcceleration += alignment(boid)  * config::alignmentScalar;
      newAcceleration += cohesion(boid)   * config::cohesionScalar;
      newAcceleration += separation(boid) * config::separationScalar;

      // avoid collisions
      newAcceleration += avoidObstacles(boid, obstacles);
      
      // To make the acceleration softer(more realistic).
      newAcceleration *= 0.005f;
      
      // We update the velocity
      boid.velocity += newAcceleration;
      
      // We have to limit the velocity because 
      // if not, it will increase infinitly.
      boid.velocity = util::clampMag(boid.velocity, config::MAX_SPEED);
      
      // If we changed the velocity it means that it is almost
      // for sure that we changed the direction, so we have
      // to rotate the vertices of the triangle.
      if (!util::isZeroVector(newAcceleration))
         boid.rotate();
      
      boid.moveFoward();
      boid.keepInsideBorders();
   }
}

glm::vec2 Flock::avoidObstacles(
      const Boid& boid, 
      const std::vector<Obstacle> &obstacles
){
   glm::vec2 avg = glm::vec2(0.0);
   int nNeighbords = 0;

   for (const auto& obstacle : obstacles)
   {
         float d = glm::length(obstacle.center - boid.center) - obstacle.radius;

         if (d < 0.18)
         {
            glm::vec2 oppositeDir = boid.center - obstacle.center;
            oppositeDir /= d;

            avg += oppositeDir;
            nNeighbords++;
         }
   }

   if (nNeighbords == 0)
      return avg;

   glm::vec2 avgVelocity = avg / float(nNeighbords);

   avgVelocity = util::setMag(avgVelocity, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(avgVelocity, boid.velocity);

   return steering;
}

glm::vec2 Flock::alignment(const Boid& boid)
{
   glm::vec2 avgVelocity = getAverageVector(boid, 0);

   if (util::isZeroVector(avgVelocity))
      return avgVelocity;

   avgVelocity = util::setMag(avgVelocity, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(avgVelocity, boid.velocity);

   return steering;
}

// Center of mass -> avgPosition
glm::vec2 Flock::cohesion(const Boid& boid)
{
   glm::vec2 avgPosition = getAverageVector(boid, 1);

   if (avgPosition == glm::vec2(0.0))
      return avgPosition;

   glm::vec2 toCenterOfMass = avgPosition - boid.center;
   toCenterOfMass = util::setMag(toCenterOfMass, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(toCenterOfMass, boid.velocity);
   
   return steering;
}

glm::vec2 Flock::separation(const Boid& boid)
{
   glm::vec2 avgVelocity = getAverageVector(boid, 2);

   if (avgVelocity == glm::vec2(0.0))
      return avgVelocity;

   avgVelocity = util::setMag(avgVelocity, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(avgVelocity, boid.velocity);

   return steering;
}

// Type == 0: Alignment
// Type == 1: Cohesion
// Type == 2: Separation
glm::vec2 Flock::getAverageVector(const Boid& boid, const int type)
{
   glm::vec2 avg = glm::vec2(0.0);
   int nNeighbords = 0;

   for (const auto& otherBoid : boids)
   {
      if (boid.id == otherBoid.id)
         continue;
      
      float range = std::abs(util::getAngle(boid.velocity, otherBoid.velocity));
      float d = glm::length(boid.center - otherBoid.center);

      // ~Alignment~
      // We need to calculate the average velocity.
      if (type == 0)
      {
         if (d < 0.19 && range < 1.57) {
            avg += otherBoid.velocity;
            nNeighbords++;
         }

      } 

      // ~Cohesion~
      // We need to calculate the average position.
      if (type == 1)
      {
         if (d < 0.20 && range < 1.57)
         {
            avg += otherBoid.center;
            nNeighbords++;
         }
      }

      // ~Separation~
      if (type == 2)
      {
         if (d < 0.18 && range < 1.57)
         {
            glm::vec2 oppositeDir = boid.center - otherBoid.center;
            oppositeDir /= d;

            avg += oppositeDir;
            nNeighbords++;
         }
      }

   }

   if (nNeighbords == 0)
      return avg;
   else 
      return (avg / float(nNeighbords));
}

glm::vec3 Flock::pickRandColor()
{
   std::uniform_real_distribution<> rand(0.0, 1.0);

   return glm::vec3(
         rand(config::gen),
         rand(config::gen),
         rand(config::gen)
   );
}

int Flock::getSize()
{
   return boids.size();
}
