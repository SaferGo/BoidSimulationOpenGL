#include <boidSimulation/WindowManager.h>

#include <iostream>
#include <exception>

#include <SDL2/SDL.h>

WindowManager* WindowManager::m_instance = nullptr;

WindowManager::WindowManager(
      const unsigned int width,
      const unsigned int height,
      const char* title
){
      this->initSDL();
      
      enableMSAA();

      m_window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
      );
      
      if (!m_window) {
         throw std::runtime_error(
               strcat(
                  (char*)"Failed to create SDL2 window.\nError: ",
                  SDL_GetError()
               )
         );
      }
}

WindowManager* WindowManager::getInstance(
      const unsigned int width,
      const unsigned int height,
      const char* title
){
   if (m_instance == nullptr)
      m_instance = new WindowManager(width, height, title);

   return m_instance;
}

SDL_Window* WindowManager::getWindow()
{
   return m_window;
}

void WindowManager::initSDL()
{
   if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      throw std::runtime_error(
            strcat(
               (char*)"Failed to initialize the SDL2 library.\nError: ",
               SDL_GetError()
            )
      );
   }
}

void WindowManager::enableMSAA()
{
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
}

void WindowManager::swapBuffers()
{
   // Updates the window with OpenGL rendering
   SDL_GL_SwapWindow(m_window);
}

void WindowManager::destroyWindow()
{
   this->~WindowManager();
}

WindowManager::~WindowManager()
{
   SDL_DestroyWindow(m_window);
}
