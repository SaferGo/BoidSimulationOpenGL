#include <boidSimulation/Obstacle.h>

#include <boidSimulation/config.h>

Obstacle::Obstacle()
{
   std::uniform_real_distribution<> randPos(-1.0, 1.0);
   std::uniform_real_distribution<> randRad(0.02, 0.2);
   std::uniform_real_distribution<> randColor(0.0, 1.0);

   center = glm::vec2(randPos(config::gen), randPos(config::gen));
   radius = randRad(config::gen);
   color[0] = color[1] = color[2] = glm::vec3(
            randColor(config::gen),
            randColor(config::gen),
            randColor(config::gen)
   );

   createTrianglesOfCircle();
}

void Obstacle::createTrianglesOfCircle()
{
   float pi2 = 3.141592f * 2.0f;
   glm::vec2 newPoint = center;

   for (int i = 0; i < config::N_TRIANG_PER_CIRCLE; i++)
   {
      pos[i][0] = center;

      for (int j = 1; j <= 2; j++)
      {
         float angle = pi2 * (i + j - 1) / 30.0f;

         newPoint.x = center.x + cos(angle) * radius;
         newPoint.y = center.y + sin(angle) * radius;

         pos[i][j] = newPoint;
      }
   }
}
