#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <boidSimulation/Window/WindowManager.h>
#include <boidSimulation/Renderer/Renderer.h>
#include <boidSimulation/Shaders/glShaderLoader.h>
#include <boidSimulation/Settings/config.h>
#include <boidSimulation/Simulation/Flock.h>
#include <boidSimulation/Simulation/Obstacle.h>

class Simulation
{

public:

   Simulation();
   ~Simulation();

   void run();

private:

   std::vector<Flock> m_flocks;
   std::vector<Obstacle> m_obstacles;

   bool m_appRunning;
   unsigned int m_FPS;
   
   WindowManager* m_window;
   Renderer*      m_renderer;
   SDL_GLContext  m_glContext;
   unsigned int   m_shaderProgram;

   unsigned int m_bufferVertex, m_bufferColor;
   unsigned int m_VAO;

   // ========================== Methods ==========================

   void update();
   void render();
   void checkUserInput();
   void calculateFPS();
   void createFlocks();
   void createObstacles();
   void updateFlocks();

   // OpenGL functions
   void initGLAD();
   void createBuffer(
         const size_t bufferSize,
         unsigned int& buffer,
         const GLenum bufferType,
         const GLenum drawingType
   );
   void bindBufferToVAO(
      const unsigned int vao,
      const unsigned int index,
      const unsigned int buffer,
      const unsigned int offset,
      const unsigned int stride
   );
   void bindVertexAttribute(
      const unsigned int vao,
      const unsigned int vaoIndex,
      const unsigned int shaderAttribIndex,
      const unsigned int nComponents,
      const GLenum type,
      const GLenum isNormalized,
      const unsigned int distance
   );
   void updateBoidChunk();
   void updateChunkOfBuffer(
         const unsigned int buffer,
         const unsigned int vertexOffset,
         const unsigned int sizeData,
         void* data
   );
   void initBuffers();
   void createVertexBuffer();
   void createColorBuffer();
   void fillObstacleChunk();
   void fillColorBoidBuffer();
   void initShaders();

   // ImGui functions
   void initImGui();
   void updateGUI();

};
