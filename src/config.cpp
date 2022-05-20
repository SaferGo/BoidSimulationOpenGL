#include <boidSimulation/config.h>

std::mt19937 config::gen = std::mt19937(time(0));

constexpr char* config::WINDOW_TITLE = "Boid Simulator";
const int   config::RESOLUTION_W = 1080;
const int   config::RESOLUTION_H = 880;

const float config::B_WIDTH  = 0.01;
const float config::B_HEIGHT = 0.03;

const int   config::MAX_N_SPECIES   = 20;
const int   config::MAX_N_BOIDS     = 50;
const int   config::N_TRIANG_PER_CIRCLE = 6;
const int   config::MAX_N_OBSTACLES = 1;
const float config::MAX_SPEED       = 0.0095;
const float config::MIN_SPEED       = 0.0045;
const float config::MAX_MAG         = 0.055;

// Default values which we can change in the GUI
float config::cohesionScalar   = 1.0f;
float config::separationScalar = 1.0f;
float config::alignmentScalar  = 1.0f;
