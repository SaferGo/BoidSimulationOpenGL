#ifndef BOID_H
#define BOID_H

#include <glm/glm.hpp>

class Boid
{

public:

   // Pos: All the postions of the vertices of a triangle/boid
   // pos[0] -> left tail of triangle
   // pos[1] -> right tail of triangle
   // pos[2] -> top of the triangle
   // Also the boid has an acceleration which we gonna use it 
   // to change the velocity in every update (remember that 
   // acceleration is when the velocity changes). We gonna
   // update the acceleration in the flock function.
   glm::vec2 pos[3];
   glm::vec2 center;
   glm::vec2 velocity;
   int id;
      
   // ========================== Methods ==========================

   Boid(const int newID);
   ~Boid();

   void moveFoward();
   void rotate();
   void keepInsideBorders();


private:

   bool isOutOfBorder(const glm::vec2 pos);
   bool isOutOfScreen();
   
};


#endif
