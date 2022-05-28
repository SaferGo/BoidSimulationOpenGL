#include <boidSimulation/Boid.h>

#include <boidSimulation/util.h>
#include <boidSimulation/config.h>

Boid::Boid(const int newID) : _id(newID)
{
   std::uniform_real_distribution<> rand(-1.0, 1.0);
   
   _center = glm::vec2(rand(config::gen), rand(config::gen));
   _velocity = util::generateRandVelocity();

   rotate();
}

void Boid::rotate()
{
   glm::vec2 direction = glm::normalize(_velocity);

   // Top vertex
   _pos[0] = _center + direction * config::B_HEIGHT;
   // Left tail vertex
   _pos[1] = _center + glm::vec2(-direction.y, direction.x) * config::B_WIDTH;
   // Right tail vertex
   _pos[2] = _center + glm::vec2(direction.y, -direction.x) * config::B_WIDTH;
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
      if (!isOutOfBorder(_pos[i]))
         return false;
   }

   if (!isOutOfBorder(_center))
      return false;

   return true;
}

void Boid::keepInsideBorders()
{
   if (isOutOfScreen())
   {
      if (_center.x >  1.0)
      {
         _center.x -= (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++)
            _pos[i].x -= (2.0 + config::B_HEIGHT);
      }
      else if (_center.x < -1.0)
      {
         _center.x += (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++) 
            _pos[i].x += (2.0 + config::B_HEIGHT);
      }

      if (_center.y >  1.0)
      {
         _center.y -= (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++) 
            _pos[i].y -= (2.0 + config::B_HEIGHT);
      }
      else if (_center.y < -1.0)
      {
         _center.y += (2.0 + config::B_HEIGHT);
         for (int i = 0; i < 3; i++)
            _pos[i].y += (2.0 + config::B_HEIGHT);
      }
   }
}

// Move all the vertices forward(with the velocity vector)
void Boid::moveFoward()
{
   _center += _velocity;
   for (int i = 0; i < 3; i++)
      _pos[i] += _velocity;
}

Boid::~Boid() = default;
