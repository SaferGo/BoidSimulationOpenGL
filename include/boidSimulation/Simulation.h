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

class Simulation
{

public:

   Simulation(const int nSpecies, const int nBoids);
   ~Simulation();

   void run();

private:

   std::vector<Flock> flocks;
   bool appRunning;
   
   WindowManager* window;
   Renderer*      renderer;
   SDL_GLContext  glContext;
   unsigned int   shaderProgram;

   unsigned int buffer;
   unsigned int VAO;

   // ========================== Methods ==========================

   void init();
   void update();
   void render();
   void checkUserInput();

   // OpenGL functions
   void initGLAD();
   void updateChunkOfBuffer(const int i, glm::vec2* data);
   void updateAllBuffer();
   void initShaders();
   void setupVertices();

   // ImGui functions
   void initImGui();
   void updateGUI();

};

#endif
