#include <SDL/SDL.h>
#include <iostream>

int main()
{
   if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cout << "Failed to initialize the SDL2 library(for video)\n";
      std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
      return -1;
   }

   SDL_Window* window = SDL_CreateWindow("Boid Simulation",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          680,
                                          480,
                                          0);

   if (!window) {
      std::cout << "Failed to create window\n";
      std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
      return -1;
   }

   SDL_Surface* window_surface = SDL_GetWindowSurface(window);

   if (!window_surface) {
      std::cout << "Failed to get the surface from the windows\n";
      std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
      return -1;
   }

   SDL_UpdateWindowSurface(window);
   SDL_Delay(5000);
	return 0;
}
