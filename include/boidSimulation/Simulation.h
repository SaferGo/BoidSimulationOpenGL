#pragma once

#include <iostream>
#include <vector>
#include <string>

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

   std::vector<Flock> _flocks;
   std::vector<Obstacle> _obstacles;

   bool _appRunning;
   unsigned int _FPS;
   
   WindowManager* _window;
   Renderer*      _renderer;
   SDL_GLContext  _glContext;
   unsigned int   _shaderProgram;

   unsigned int _bufferVertex, _bufferColor;
   unsigned int _VAO;

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
