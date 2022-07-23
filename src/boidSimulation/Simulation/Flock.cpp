#include <boidSimulation/Simulation/Flock.h>

#include <boidSimulation/Settings/config.h>
#include <boidSimulation/Math/util.h>

#define ALIGNMENT  0
#define COHESION   1
#define SEPARATION 2

Flock::Flock()
{
   m_boids.reserve(config::MAX_N_BOIDS);
   m_color[0] = m_color[1] = m_color[2] = pickRandColor();
   
   for (int i = 0; i < config::MAX_N_BOIDS; i++)
      m_boids.push_back(Boid(i));
}

void Flock::toFlock(const std::vector<Obstacle>& obstacles)
{
   for (auto& boid: m_boids)
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
      boid.m_velocity += newAcceleration;
      
      // Limits the velocity because if not, it will increase infinitly.
      boid.m_velocity = util::clampMag(boid.m_velocity, config::MAX_SPEED);
      
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
         float d = fabs(glm::length(obstacle.m_center - boid.m_center));

         if (d - 0.045f < obstacle.m_radius)
         {
            glm::vec2 oppositeDir = boid.m_center - obstacle.m_center;
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
         boid.m_velocity
   );

   // The steering force has to be big when the boids hit a obstacle.
   return steering * 3.0f;
}

glm::vec2* Flock::getBoidPosition(const int i)
{
   return &m_boids[i].m_pos[0];
}

glm::vec3* Flock::getColor()
{
   return &m_color[0];
}

// Calculates the average velocity.
glm::vec2 Flock::alignment(const Boid& boid) const
{
   glm::vec2 avgVelocity = getAverageVector(boid, ALIGNMENT);

   if (util::isZeroVector(avgVelocity))
      return avgVelocity;

   avgVelocity = util::setMag(avgVelocity, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(
         avgVelocity, boid.m_velocity
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

   glm::vec2 toCenterOfMass = avgPosition - boid.m_center;
   toCenterOfMass = util::setMag(toCenterOfMass, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(
         toCenterOfMass, boid.m_velocity
   );
   
   return steering;
}

// Keeps distance between the boids of the same specie.
glm::vec2 Flock::separation(const Boid& boid) const
{
   glm::vec2 avgVelocity = getAverageVector(boid, SEPARATION);

   if (avgVelocity == glm::vec2(0.0))
      return avgVelocity;

   avgVelocity = util::setMag(avgVelocity, config::MAX_SPEED);

   glm::vec2 steering = util::getSteeringVector(avgVelocity, boid.m_velocity);

   return steering;
}

glm::vec2 Flock::getAverageVector(const Boid& boid, const int type) const
{
   glm::vec2 avg = glm::vec2(0.0);
   int nNeighbords = 0;

   for (const auto& other : m_boids)
   {
      if (boid.m_id == other.m_id)
         continue;
      
      float range = std::abs(util::getAngle(boid.m_velocity, other.m_velocity));
      float d = glm::length(boid.m_center - other.m_center);

      if (type == ALIGNMENT)
      {
         if (d < config::ALIGNMENT_DIST && range < config::ALIGNMENT_RANGE) {
            avg += other.m_velocity;
            nNeighbords++;
         }

      } 

      if (type == COHESION)
      {
         if (d < config::COHESION_DIST && range < config::COHESION_RANGE)
         {
            avg += other.m_center;
            nNeighbords++;
         }
      }

      // ~Separation~
      if (type == SEPARATION)
      {
         if (d < config::SEPARATION_DIST && range < config::SEPARATION_RANGE)
         {
            glm::vec2 oppositeDir = boid.m_center - other.m_center;
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
