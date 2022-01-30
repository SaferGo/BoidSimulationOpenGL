#include <boidSimulation/Simulation.h>

int main() 
{
   // config::MAX_N_BOIDS = 80
   Simulation simulation = Simulation(1, 80);

   simulation.run();

	return 0;
}
