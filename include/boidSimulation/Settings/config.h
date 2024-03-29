#pragma once

#include <random>

namespace config
{
   inline constexpr char* WINDOW_TITLE   = (char*)"Boid Simulator";
   inline constexpr int   RESOLUTION_W   = 1080;
   inline constexpr int   RESOLUTION_H   = 880;

   inline constexpr float B_WIDTH  = 0.01;
   inline constexpr float B_HEIGHT = 0.03;

   inline constexpr int   MAX_N_SPECIES       = 10;
   inline constexpr int   MAX_N_BOIDS         = 63;
   inline constexpr int   MAX_N_OBSTACLES     = 30;
   inline constexpr int   N_TRIANG_PER_CIRCLE = 30;
   inline constexpr float MAX_SPEED           = 0.0095;
   inline constexpr float MIN_SPEED           = 0.0045;
   inline constexpr float MAX_MAG             = 0.055;
   
   inline constexpr float ALIGNMENT_DIST   = 0.19;
   inline constexpr float ALIGNMENT_RANGE  = 1.57;
   inline constexpr float COHESION_DIST    = 0.20;
   inline constexpr float COHESION_RANGE   = 1.57;
   inline constexpr float SEPARATION_DIST  = 0.18;
   inline constexpr float SEPARATION_RANGE = 1.57;
   
   inline float cohesionScalar   = 1.0f;
   inline float separationScalar = 1.0f;
   inline float alignmentScalar  = 1.0f;

   inline std::mt19937 gen = std::mt19937(time(0));
}
