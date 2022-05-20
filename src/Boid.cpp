#include <boidSimulation/Boid.h>

#include <boidSimulation/util.h>
#include <boidSimulation/config.h>

Boid::Boid(const int newID) : id(newID)
{
   std::uniform_real_distribution<> rand(-1.0, 1.0);
   
   center = glm::vec2(rand(config::gen), rand(config::gen));
   velocity = util::generateRandVelocity();

   rotate();
}

void Boid::rotate()
{
   glm::vec2 direction = glm::normalize(velocity);

   // Top vertex
   pos[0] = center + direction * config::B_HEIGHT;
   // Left tail vertex
   pos[1] = center + glm::vec2(-direction.y, direction.x) * config::B_WIDTH;
   // Right tail vertex
   pos[2] = center + glm::vec2(direction.y, -direction.x) * config::B_WIDTH;
}


bool Boid::isOutOfBorder(const glm::vec2 v)
{
   if (v.x > 1.0 || v.x < -1.0 || v.y > 1.0 || v.y < -1.0)
      return true;

   return false;
}

bool Boid::isOutOfScreen()
{
   for (int i = 0; i < 3; i++)
   {
      if (!isOutOfBorder(pos[i]))
         return false;
   }

   if (!isOutOfBorder(center))
      return false;

   return true;
}

void Boid::keepInsideBorders()
{
   if (isOutOfScreen())
   {
      if (center.x >  1.0)
      {
         center.x -= (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++)
            pos[i].x -= (2.0 + config::B_HEIGHT);
      }
      else if (center.x < -1.0)
      {
         center.x += (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++) 
            pos[i].x += (2.0 + config::B_HEIGHT);
      }

      if (center.y >  1.0)
      {
         center.y -= (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++) 
            pos[i].y -= (2.0 + config::B_HEIGHT);
      }
      else if (center.y < -1.0)
      {
         center.y += (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++)
            pos[i].y += (2.0 + config::B_HEIGHT);
      }
   }
}

// Move all the vertices forward(with the velocity vector)
void Boid::moveFoward()
{
   center += velocity;
   for (int i = 0; i < 3; i++)
      pos[i] += velocity;
}

Boid::~Boid() = default;
