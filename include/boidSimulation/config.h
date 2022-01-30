#ifndef CONFIG_H
#define CONFIG_H

#include <random>

namespace config
{
   extern std::mt19937 gen;

   extern const char* WINDOW_TITLE;
   extern const int   RESOLUTION_W;
   extern const int   RESOLUTION_H;

   extern const float B_WIDTH;
   extern const float B_HEIGHT;

   extern const int   MAX_N_BOIDS;
   extern const float MAX_SPEED;
   extern const float MIN_SPEED;
   extern const float MAX_MAG;

   extern float cohesionScalar;
   extern float separationScalar;
   extern float alignmentScalar;
}

#endif
