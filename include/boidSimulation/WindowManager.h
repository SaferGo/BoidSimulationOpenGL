#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <SDL2/SDL.h>

class WindowManager
{
   
public:
   static WindowManager* getInstance(
         const unsigned int width,
         const unsigned int height,
         const char* title
   );
   SDL_Window* getWindow();
   void swapBuffers();
   void destroyWindow();

private:

   static WindowManager* instance;
   SDL_Window* window;

   // ========================== Methods ==========================

   WindowManager(
         const unsigned int width,
         const unsigned int height,
         const char* title
   );
   ~WindowManager();
   void initSDL();

   WindowManager(const WindowManager& windowManager) = delete;
   WindowManager* operator=(const WindowManager& windowManager) = delete;

};

#endif
