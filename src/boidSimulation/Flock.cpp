#include <boidSimulation/Flock.h>

#include <boidSimulation/config.h>
#include <boidSimulation/util.h>

#define ALIGNMENT  0
#define COHESION   1
#define SEPARATION 2

Flock::Flock()
{
   _boids.reserve(config::MAX_N_BOIDS);
   _color[0] = _color[1] = _color[2] = pickRandColor();
   
   for (int i = 0; i < config::MAX_N_BOIDS; i++)
      _boids.push_back(Boid(i));
}

void Flock::toFlock(const std::vector<Obstacle>& obstacles)
{
   for (auto& boid: _boids)
   {
      glm::vec2 newAcceleration = glm::vec2(0.0);
      
      newAcceleration += alignment(boid)  * config::alignmentScalar;
      newAcceleration += cohesion(boid)   * config::cohesionScalar;
      newAcceleration += separation(boid) * config::separationScalar;

      // Avoids collisions
      newAcceleration += avoidObstacles(boid, obstacles);
      
      // Makes the acceleration softer(more realistic).
      newAcceleration *= 0.005f;
      
      // Updates the velocity
      boid._velocity += newAcceleration;
      
      // Limits the velocity because if not, it will increase infinitly.
      boid._velocity = util::clampMag(boid._velocity, config::MAX_SPEED);
      
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
      const Boid& boid, const std::vector<Obstacle>& obstacles) const
{

   glm::vec2 avg = glm::vec2(0.0);
   int nNeighbords = 0;

   for (const auto& obstacle : obstacles)
   {
         float d = fabs(glm::length(obstacle._center - boid._center));

         if (d - 0.045f < obstacle._radius)
         {
            glm::vec2 oppositeDir = boid._center - obstacle._center;
            oppositeDir /= d;

            avg += oppositeDir;
            nNeighbords++;
         }
   }

   if (nNeighbords == 0)
      return avg;

   glm::vec2 avgVelocity = avg / float(nNeighbords);

   avgVelocity = util::setMag(avgVelocity, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(
         avgVelocity,
         boid._velocity
   );

   // The steering force has to be big when the boids hit a obstacle.
   return steering * 3.0f;
}

glm::vec2* Flock::getBoidPosition(const int i)
{
   return &_boids[i]._pos[0];
}

glm::vec3* Flock::getColor()
{
   return &_color[0];
}

// Calculates the average velocity.
glm::vec2 Flock::alignment(const Boid& boid) const
{
   glm::vec2 avgVelocity = getAverageVector(boid, ALIGNMENT);

   if (util::isZeroVector(avgVelocity))
      return avgVelocity;

   avgVelocity = util::setMag(avgVelocity, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(
         avgVelocity, boid._velocity
   );

   return steering;
}

// Center of mass -> avgPosition
// Calculates the average position.
glm::vec2 Flock::cohesion(const Boid& boid) const
{
   glm::vec2 avgPosition = getAverageVector(boid, COHESION);

   if (avgPosition == glm::vec2(0.0))
      return avgPosition;

   glm::vec2 toCenterOfMass = avgPosition - boid._center;
   toCenterOfMass = util::setMag(toCenterOfMass, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(toCenterOfMass, boid._velocity);
   
   return steering;
}

// Keeps distance between the boids of the same specie.
glm::vec2 Flock::separation(const Boid& boid) const
{
   glm::vec2 avgVelocity = getAverageVector(boid, SEPARATION);

   if (avgVelocity == glm::vec2(0.0))
      return avgVelocity;

   avgVelocity = util::setMag(avgVelocity, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(avgVelocity, boid._velocity);

   return steering;
}

glm::vec2 Flock::getAverageVector(const Boid& boid, const int type) const
{
   glm::vec2 avg = glm::vec2(0.0);
   int nNeighbords = 0;

   for (const auto& other : _boids)
   {
      if (boid._id == other._id)
         continue;
      
      float range = std::abs(util::getAngle(boid._velocity, other._velocity));
      float d = glm::length(boid._center - other._center);

      if (type == ALIGNMENT)
      {
         if (d < config::ALIGNMENT_DIST && range < config::ALIGNMENT_RANGE) {
            avg += other._velocity;
            nNeighbords++;
         }

      } 

      if (type == COHESION)
      {
         if (d < config::COHESION_DIST && range < config::COHESION_RANGE)
         {
            avg += other._center;
            nNeighbords++;
         }
      }

      // ~Separation~
      if (type == SEPARATION)
      {
         if (d < config::SEPARATION_DIST && range < config::SEPARATION_RANGE)
         {
            glm::vec2 oppositeDir = boid._center - other._center;
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

glm::vec3 Flock::pickRandColor() const
{
   std::uniform_real_distribution<> rand(0.0, 1.0);

   return glm::vec3(
         rand(config::gen),
         rand(config::gen),
         rand(config::gen)
   );
}

Flock::~Flock() = default;
