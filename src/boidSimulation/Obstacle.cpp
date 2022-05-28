#include <boidSimulation/Obstacle.h>

#include <boidSimulation/config.h>

Obstacle::Obstacle(const unsigned id) : _id(id)
{
   std::uniform_real_distribution<> randPos(-1.0, 1.0);
   std::uniform_real_distribution<> randRad(0.05, 0.1);
   std::uniform_real_distribution<> randColor(0.0, 1.0);

   _center = glm::vec2(randPos(config::gen), randPos(config::gen));
   _radius = randRad(config::gen);
   _color[0] = _color[1] = _color[2] = glm::vec3(
            randColor(config::gen),
            randColor(config::gen),
            randColor(config::gen)
   );

   createTrianglesOfCircle();
}

void Obstacle::createTrianglesOfCircle()
{
   const float pi2 = 3.141592f * 2.0f;
   glm::vec2 newPoint = _center;

   for (int i = 0; i < config::N_TRIANG_PER_CIRCLE; i++)
   {
      _pos[i][0] = _center;

      for (int j = 1; j <= 2; j++)
      {
         float angle = pi2 * (i + j - 1) / 30.0f;

         newPoint.x = _center.x + cos(angle) * _radius;
         newPoint.y = _center.y + sin(angle) * _radius;

         _pos[i][j] = newPoint;
      }
   }
}

bool Obstacle::doesCollide(const std::vector<Obstacle>& obstacles) const
{
   for (const auto& other : obstacles)
   {
      if (_id == other._id)
         continue;

      float sumOfRadius = other._radius + _radius;
      float distance = fabs(
            glm::distance(other._center, _center)
      ) - sumOfRadius;

      if (distance <= 0.0)
         return true;
   }

   return false;
}

glm::vec3* Obstacle::getColor()
{
   return &(_color[0]);
}

glm::vec2* Obstacle::getPos(const unsigned int i)
{
   return &(_pos[i][0]);
}
