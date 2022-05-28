#pragma once

#include <glm/glm.hpp>

class Boid
{

friend class Flock;

public:

   Boid(const int newID);
   ~Boid();

   void moveFoward();
   void rotate();
   void keepInsideBorders();


private:

   // Pos: All the postions of the vertices of a triangle/boid
   // pos[0] -> left tail of triangle
   // pos[1] -> right tail of triangle
   // pos[2] -> top of the triangle
   // Also the boid has an acceleration which we gonna use it to change the 
   // velocity in every update (remember that acceleration is when the 
   // velocity changes). We gonna update the acceleration in the flock 
   // function.
   glm::vec2 _pos[3];
   glm::vec2 _center;
   glm::vec2 _velocity;
   int _id;

   // ========================== Methods ==========================

   bool isOutOfBorder(const glm::vec2 _pos) const;
   bool isOutOfScreen() const;
   
};
