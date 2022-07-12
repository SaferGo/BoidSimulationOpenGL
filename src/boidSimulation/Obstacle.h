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
   glm::vec2 m_pos[config::N_TRIANG_PER_CIRCLE][3];
   glm::vec2 m_center;
   float m_radius;
   glm::vec3 m_color[3];
   unsigned m_id;

   // ========================== Methods ==========================

   void createTrianglesOfCircle();
};
