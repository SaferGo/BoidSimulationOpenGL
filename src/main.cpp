#include <glad/glad.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <exception>
#include <string.h>

void initGLAD()
{
   // Here we initialize GLAD and loads all the GL function pointers.
   if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
      throw std::runtime_error("Failed to initialize GLAD.");
}

void initSDL()
{
   if (SDL_Init(SDL_INIT_VIDEO) < 0)
      throw std::runtime_error(
               strcat(
                  (char*)"Failed to initialize the SDL2 library.\nError: ",
                  SDL_GetError()
               )
      );
}

void initWindow(SDL_Window*& window)
{
   window = SDL_CreateWindow("Boid Simulation",
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             680,
                             480,
                             SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

   if (!window)
      throw std::runtime_error(
               strcat(
                  (char*)"Failed to create SDL2 window.\nError: ",
                  SDL_GetError()
               )
      );
}

void initRenderer(SDL_Renderer*& renderer, SDL_Window*& window)
{
   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

   if (!renderer)
      throw std::runtime_error(
               strcat(
                  (char*)"Failed to create SDL2 renderer.\nError: ",
                  SDL_GetError()
               )
      );
}

int main() 
{

   SDL_Window* window;
   SDL_Renderer* renderer;

   try {
      initSDL();
      initWindow(window);
      initRenderer(renderer, window);
      initGLAD();
   } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;

      return -1;
   }
   
   SDL_GLContext glContext = SDL_GL_CreateContext(window);

   SDL_Event event;

   bool running = true;

   while (running) {
      /*if (SDL_PollEvent(&event) > 0)
         std::cout << "Input Detected!\n";
         */

      glClearColor(1, 0, 0, 1);

      // Updates a window with OpenGL rendering
      SDL_GL_SwapWindow(window);
   }

   SDL_GL_DeleteContext(glContext);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);

	return 0;
}
