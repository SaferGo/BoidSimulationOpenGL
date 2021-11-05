#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>

class Renderer
{

public:
   static Renderer* getInstance(SDL_Window* window);
   SDL_Renderer* getRenderer();
   void destroyRenderer();

private:
   static Renderer* instance;
   SDL_Renderer* renderer;

   Renderer(SDL_Window* window);
   Renderer(const Renderer&) = delete;
   Renderer& operator=(const Renderer&) = delete;
   ~Renderer();
};

#endif
