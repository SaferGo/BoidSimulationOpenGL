#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <vector>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <boidSimulation/WindowManager.h>
#include <boidSimulation/Renderer.h>
#include <boidSimulation/glShaderLoader.h>
#include <boidSimulation/config.h>
#include <boidSimulation/Flock.h>
#include <boidSimulation/Obstacle.h>

class Simulation
{

public:

   Simulation();
   ~Simulation();

   void run();

private:

   std::vector<Flock> flocks;
   std::vector<Obstacle> obstacles;
   bool appRunning;
   
   WindowManager* window;
   Renderer*      renderer;
   SDL_GLContext  glContext;
   unsigned int   shaderProgram;

   unsigned int bufferVertex, bufferColor;
   unsigned int VAO;

   // ========================== Methods ==========================

   void init();
   void update();
   void render();
   void checkUserInput();

   // OpenGL functions
   void initGLAD();
   void updateBoidChunk();
   void updateChunkOfBuffer(
         const unsigned int buffer,
         const unsigned int vertexOffset,
         const unsigned int sizeData,
         void* data
   );
   void initBuffers();
   void createObjectBuffer();
   void createColorBuffer();
   void fillObstacleChunk();
   void fillColorBuffer();

   void initShaders();

   // ImGui functions
   void initImGui();
   void updateGUI();

};

#endif
