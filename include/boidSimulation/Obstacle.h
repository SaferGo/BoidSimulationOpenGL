#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <vector>

#include <glm/glm.hpp>

#include <boidSimulation/Boid.h>
#include <boidSimulation/config.h>

class Obstacle
{

public:
   // All vertices that compose the triangle
   glm::vec2 pos[config::N_TRIANG_PER_CIRCLE][3];
   glm::vec2 center;
   float radius;
   glm::vec3 color[3];

   // ========================== Methods ==========================

   Obstacle();

private:

   void createTrianglesOfCircle();
};

#endif
