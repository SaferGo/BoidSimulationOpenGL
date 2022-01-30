#include <boidSimulation/config.h>

std::mt19937 config::gen = std::mt19937(time(0));

const char* config::WINDOW_TITLE = "Boid Simulator";
const int   config::RESOLUTION_W = 1080;
const int   config::RESOLUTION_H = 880;

const float config::B_WIDTH  = 0.01;
const float config::B_HEIGHT = 0.03;

const int   config::MAX_N_BOIDS = 80;
const float config::MAX_SPEED   = 0.004;
const float config::MIN_SPEED   = 0.002;
const float config::MAX_MAG     = 0.005;

// Default values which we can change in the GUI
float config::cohesionScalar   = 1.0f;
float config::separationScalar = 1.0f;
float config::alignmentScalar  = 1.0f;
