#include <boidSimulation/Simulation/Obstacle.h>

#include <boidSimulation/Settings/config.h>

Obstacle::Obstacle(const unsigned id) : m_id(id)
{
   std::uniform_real_distribution<> randPos(0.0, 1.0);
   std::uniform_real_distribution<> randRad(0.05, 0.1);
   std::uniform_real_distribution<> randColor(0.0, 1.0);

   m_center = glm::vec2(
         (randPos(config::gen) * 2.0) - 1.0,
         (randPos(config::gen) * 2.0) - 1.0
   );
   m_radius = randRad(config::gen);
   m_color[0] = m_color[1] = m_color[2] = glm::vec3(
            randColor(config::gen),
            randColor(config::gen),
            randColor(config::gen)
   );

   createTrianglesOfCircle();
}

void Obstacle::createTrianglesOfCircle()
{
   const float pi2 = 3.141592f * 2.0f;
   glm::vec2 newPoint = m_center;

   for (int i = 0; i < config::N_TRIANG_PER_CIRCLE; i++)
   {
      m_pos[i][0] = m_center;

      for (int j = 1; j <= 2; j++)
      {
         float angle = pi2 * (i + j - 1) / 30.0f;

         newPoint.x = m_center.x + cos(angle) * m_radius;
         newPoint.y = m_center.y + sin(angle) * m_radius;

         m_pos[i][j] = newPoint;
      }
   }
}

bool Obstacle::doesCollide(const std::vector<Obstacle>& obstacles) const
{
   for (const auto& other : obstacles)
   {
      if (m_id == other.m_id)
         continue;

      float sumOfRadius = other.m_radius + m_radius;
      float distance = fabs(
            glm::distance(other.m_center, m_center)
      ) - sumOfRadius;

      if (distance <= 0.0)
         return true;
   }

   return false;
}

glm::vec3* Obstacle::getColor()
{
   return &(m_color[0]);
}

glm::vec2* Obstacle::getPos(const unsigned int i)
{
   return &(m_pos[i][0]);
}
