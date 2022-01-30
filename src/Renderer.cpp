#include <boidSimulation/Renderer.h>

#include <iostream>

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(SDL_Window* window)
{
   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

   if (!renderer) {
      throw std::runtime_error(
               strcat(
                  (char*)"Failed to create SDL2 renderer.\nError: ",
                  SDL_GetError()
               )
      );
   }

}

Renderer* Renderer::getInstance(SDL_Window* window)
{
   if (instance == nullptr)
      instance = new Renderer(window);

   return instance;
}

SDL_Renderer* Renderer::getRenderer()
{
   return renderer;
}

void Renderer::destroyRenderer()
{
   this->~Renderer();
}

Renderer::~Renderer()
{
   SDL_DestroyRenderer(renderer);
}
