#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <boidSimulation/Boid.h>
#include <boidSimulation/config.h>

class Obstacle
{

friend class Flock;

public:
   
   Obstacle(const unsigned id);
   bool doesCollide(const std::vector<Obstacle>& obstacles) const;
   glm::vec3* getColor();
   glm::vec2* getPos(const unsigned int i);
   
private:

   // All vertices that compose the triangle
   glm::vec2 _pos[config::N_TRIANG_PER_CIRCLE][3];
   glm::vec2 _center;
   float _radius;
   glm::vec3 _color[3];
   unsigned _id;

   // ========================== Methods ==========================

   void createTrianglesOfCircle();
};
