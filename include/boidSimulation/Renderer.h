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

   // ========================== Methods ==========================
   Renderer(SDL_Window* window);
   Renderer(const Renderer& other) = delete;
   Renderer& operator=(const Renderer& other) = delete;
   ~Renderer();
};

#endif
