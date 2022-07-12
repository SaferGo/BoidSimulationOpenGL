#include <boidSimulation/Boid.h>

#include <boidSimulation/util.h>
#include <boidSimulation/config.h>

Boid::Boid(const int newID) : m_id(newID)
{
   std::uniform_real_distribution<> rand(0, 1.0);
   
   m_center = glm::vec2(
         (rand(config::gen) * 2.0) - 1.0,
         (rand(config::gen) * 2.0) - 1.0);
   m_velocity = util::generateRandVelocity();

   rotate();
}

void Boid::rotate()
{
   glm::vec2 direction = glm::normalize(m_velocity);

   // Top vertex
   m_pos[0] = m_center + direction * config::B_HEIGHT;
   // Left tail vertex
   m_pos[1] = m_center + glm::vec2(-direction.y, direction.x) * config::B_WIDTH;
   // Right tail vertex
   m_pos[2] = m_center + glm::vec2(direction.y, -direction.x) * config::B_WIDTH;
}


bool Boid::isOutOfBorder(const glm::vec2 v) const
{
   if (v.x > 1.0 || v.x < -1.0 || v.y > 1.0 || v.y < -1.0)
      return true;

   return false;
}

bool Boid::isOutOfScreen() const
{
   for (int i = 0; i < 3; i++)
   {
      if (!isOutOfBorder(m_pos[i]))
         return false;
   }

   if (!isOutOfBorder(m_center))
      return false;

   return true;
}

void Boid::keepInsideBorders()
{
   if (isOutOfScreen())
   {
      if (m_center.x >  1.0)
      {
         m_center.x -= (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++)
            m_pos[i].x -= (2.0 + config::B_HEIGHT);
      }
      else if (m_center.x < -1.0)
      {
         m_center.x += (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++) 
            m_pos[i].x += (2.0 + config::B_HEIGHT);
      }

      if (m_center.y >  1.0)
      {
         m_center.y -= (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++) 
            m_pos[i].y -= (2.0 + config::B_HEIGHT);
      }
      else if (m_center.y < -1.0)
      {
         m_center.y += (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++)
            m_pos[i].y += (2.0 + config::B_HEIGHT);
      }
   }
}

// Move all the vertices forward(with the velocity vector)
void Boid::moveFoward()
{
   m_center += m_velocity;
   for (int i = 0; i < 3; i++)
      m_pos[i] += m_velocity;
}

Boid::~Boid() = default;
