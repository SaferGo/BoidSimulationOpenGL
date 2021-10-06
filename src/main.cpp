#include <glad/glad.h>
//#include <SDL2/SDL.h>

#include <iostream>

int main()
{
   if (!gladLoadGL()) {
      std::cout << "Can't load GLAD!" << std::endl;
      return -1;
   }
   std::cout << "OpenGL version loaded: "
             << GLVersion.major
             << "."
             << GLVersion.minor
             << std::endl;

   /*if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cout << "Failed to initialize the SDL2 library(for video)\n";
      std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
      return -1;
   }

   SDL_Window* window = SDL_CreateWindow("Boid Simulation",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          680,
                                          480,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

   if (!window) {
      std::cout << "Failed to create window\n";
      std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
      return -1;
   }

   SDL_GLContext glContext = SDL_GL_CreateContext(window);


   if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
      std::cout << "Failed to initialize the OpenGL context." << std::endl;
      return -1;
   }

   std::cout << "OpenGL version loaded: "
             << GLVersion.major
             << "."
             << GLVersion.minor
             << std::endl;
   //glClearColor(0.0f, 1.0f, 0.0f, 0.0f);*/
	return 0;
}
