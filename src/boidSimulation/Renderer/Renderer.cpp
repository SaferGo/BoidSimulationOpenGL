#include <boidSimulation/Renderer/Renderer.h>

#include <iostream>

Renderer* Renderer::m_instance = nullptr;

Renderer::Renderer(SDL_Window* window)
{
   m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

   if (!m_renderer) {
      throw std::runtime_error(
            strcat((char*)"Failed to create SDL2 renderer.\nError: ",
               SDL_GetError()
            )
      );
   }

}

Renderer* Renderer::getInstance(SDL_Window* window)
{
   if (m_instance == nullptr)
      m_instance = new Renderer(window);

   return m_instance;
}

SDL_Renderer* Renderer::getRenderer() const
{
   return m_renderer;
}

void Renderer::destroyRenderer()
{
   this->~Renderer();
}

Renderer::~Renderer()
{
   SDL_DestroyRenderer(m_renderer);
}
