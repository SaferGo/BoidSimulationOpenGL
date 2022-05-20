#include <boidSimulation/Simulation.h>

#include <exception>

#include <iostream>

Simulation::Simulation()
{

   // Create n species of boids
   for (int i = 0; i < config::MAX_N_SPECIES; i++)
   {
      Flock newFlock = Flock(config::MAX_N_BOIDS);
      flocks.push_back(newFlock);
   }

   // Create m obstacles
   for (int i = 0; i < config::MAX_N_OBSTACLES; i++)
   {
      Obstacle newObstacle = Obstacle();
      obstacles.push_back(newObstacle);
   }

   init();

   updateBoidChunk();
   appRunning = true;
}

Simulation::~Simulation()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext();

   SDL_GL_DeleteContext(glContext);
   renderer->destroyRenderer();
   window->destroyWindow();
}

void Simulation::run()
{
   while (appRunning) {

      checkUserInput();
      
      if (!appRunning)
         break;
      
      update();

      render();
      
      window->swapBuffers();
   }
}

void Simulation::init()
{
   
   try 
   {
      // Init Window
      window = WindowManager::getInstance(
            config::RESOLUTION_W,
            config::RESOLUTION_H,
            config::WINDOW_TITLE
      );

      // Init Renderer
      renderer = Renderer::getInstance(window->getWindow());

      initGLAD();

      glContext = SDL_GL_CreateContext(window->getWindow());

      initImGui();

   } catch (const std::exception& e)
   {
      std::cout << e.what() << std::endl;
      appRunning = false;

      return;
   }

   initShaders();
   initBuffers();
}

void Simulation::update()
{
   updateGUI();

   for (auto& flock : flocks)
      flock.toFlock(obstacles);
   
   updateBoidChunk();
}

void Simulation::render()
{
   // Render frame logic
   glClearColor(0.4, 0.1, 0.2, 1);
   glClear(GL_COLOR_BUFFER_BIT);

   // Render imgui
   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

   // Render triangles
   glUseProgram(shaderProgram);
   glBindVertexArray(VAO);
   glDrawArrays(
         GL_TRIANGLES,
         0,
         (
            (3 * config::MAX_N_BOIDS * config::MAX_N_SPECIES) +
            (3 * config::MAX_N_OBSTACLES * config::N_TRIANG_PER_CIRCLE)
         )
   );

   
   glBindVertexArray(VAO);
}

void Simulation::checkUserInput()
{
   SDL_Event event;

   while (SDL_PollEvent(&event))
   {
      ImGui_ImplSDL2_ProcessEvent(&event);

      if (event.type == SDL_QUIT ||
         (event.type == SDL_WINDOWEVENT &&
            event.window.event == SDL_WINDOWEVENT_CLOSE &&
             event.window.windowID == SDL_GetWindowID(window->getWindow())
         )
      ){
         appRunning = false;
         break;
      }
   }
}

// ========================== OpenGL functions ==========================

void Simulation::initGLAD()
{
   // Here we initialize GLAD and load all the GL function pointers.
   if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
      throw std::runtime_error("Failed to initialize GLAD.");
}

// Update the chunk of the buffer that contains the boids
void Simulation::updateBoidChunk()
{
   for (int i = 0; i < config::MAX_N_SPECIES; i++)
   {
      for (int j = 0; j < config::MAX_N_BOIDS; j++)
      {
         unsigned int vertexOffset = 
            (
             (6 * j) +
             (i * config::MAX_N_BOIDS * 6)
            ) * sizeof(float);

         updateChunkOfBuffer(
               bufferVertex, 
               vertexOffset,
               6, 
               &flocks[i].boids[j].pos
         );
      }
   }
}

void Simulation::updateChunkOfBuffer(
      const unsigned int buffer,
      const unsigned int vertexOffset,
      const unsigned int sizeData,
      void* data
){ 
   glBindVertexArray(VAO);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   
   glBufferSubData(
         GL_ARRAY_BUFFER, vertexOffset, sizeof(float) * sizeData, data
   );

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

// Build and compile the shaders.
void Simulation::initShaders()
{
   try
   {
      shaderProgram =
         glShaderLoader::loadShader(
            "../shaders/boids.vert", 
            "../shaders/boids.frag"
      );
   } catch(const std::exception& e)
   {
      std::cerr << e.what() << std::endl;
      appRunning = false;
   }
}

void Simulation::initBuffers()
{
   glGenVertexArrays(1, &VAO);
   
   createObjectBuffer();
   createColorBuffer();
   
   // Fill static buffers
   fillObstacleChunk();
   fillColorBuffer();
}

/*
// Mejorar este comentario
// typeDrawing:
// 0: Static
// 1: DYNAMIC
// 2: STREAM
void Simulation::createBuffer(
      unsigned int nElements,
      unsigned int& buffer,
      unsigned int typeDrawing
)
{
   size_t bufferSize =
      nElements * config::MAX_N_BOIDS * config::MAX_N_SPECIES * size(float);

   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   
   if (typeDrawing == 0)
      glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_STATIC);
   else if (typeDrawing == 1)
      glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_DYNAMIC);
   else
      glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STREAM_DRAW);

   glBindVertexArray(VAO);

   glEnableVertexArrayAttrib(VAO, 0);
   glVertexArrayVertexBuffer(VAO, 0, buffer, 0, sizeof(float) * 2);
   glVertexArrayAttribBinding(VAO, 0, 0);
   glVertexArrayAttribFormat(VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}
*/

void Simulation::createObjectBuffer()
{
   // Create buffer
   size_t bufferSize = 
      (
       (6 * config::MAX_N_BOIDS * config::MAX_N_SPECIES) +
       (6 * config::N_TRIANG_PER_CIRCLE * config::MAX_N_OBSTACLES)
      ) * sizeof(float);

   std::cout << "BUFFER SIZE: " << bufferSize << std::endl;
   // Create a big buffer for all the boids and the obstacles
   glGenBuffers(1, &bufferVertex);
   // We indicate the type of buffer
   glBindBuffer(GL_ARRAY_BUFFER, bufferVertex);
   // We allocate the buffer with determined size but with 0 elements.
   glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);


   // Bindeamos con el VAO
   glBindVertexArray(VAO);

   // Seleccionamos que index del VAO queremos usar para este buffer.
   // Esto se llama "vertex buffer binding index" -> indice usado en el VAO para
   // identificar un VBO.
   // (en este caso queremos usar el 0)
   // (acordate que el VAO contine los "punteros" a los VBO)
   glEnableVertexArrayAttrib(VAO, 0);
   // 1) VAO,
   // 2) Index of VBO in VAO,
   // 3) VBO,
   // 4) Offset within the VBO,
   // 5) stride(distance between the elements within the VBO).
   glVertexArrayVertexBuffer(VAO, 0, bufferVertex, 0, sizeof(float) * 2);
   // Aca hacemos la conexion con el shader.
   // Associates a vertex attribute(an input variable to a shader that is 
   // supplied with per-vertex data) and a vertex buffer binding for a vertex
   // array object.
   // 1) VAO
   // 2) Vertex attribute(el que esta en el shader).
   // 3) Vertex buffer binding index.
   glVertexArrayAttribBinding(VAO, 0, 0);
   glVertexArrayAttribFormat(VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

void Simulation::createColorBuffer()
{
   size_t bufferSize =
      (
       (9 * config::MAX_N_BOIDS * config::MAX_N_SPECIES) +
       (9 * config::N_TRIANG_PER_CIRCLE * config::MAX_N_OBSTACLES)
      ) * sizeof(float);

   glGenBuffers(1, &bufferColor);
   glBindBuffer(GL_ARRAY_BUFFER, bufferColor);
   glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);

   glBindVertexArray(VAO);

   glEnableVertexArrayAttrib(VAO, 1);
   glVertexArrayVertexBuffer(VAO, 1, bufferColor, 0, sizeof(float) * 3);
   glVertexArrayAttribBinding(VAO, bufferColor, 1);
   glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, 0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

void Simulation::fillColorBuffer()
{
   // Color-Boid chunk

   for (int i = 0; i < config::MAX_N_SPECIES; i++)
   {
      for (int j = 0; j < config::MAX_N_BOIDS; j++)
      {
         const unsigned int vertexOffset =
            (
             (9 * j) +
             (i * config::MAX_N_BOIDS * 9)
            ) * sizeof(float);

         updateChunkOfBuffer(
               bufferColor,
               vertexOffset,
               9,
               &flocks[i].color
         );
      }
   }

   // Color-Obstacle chunk
   
   const unsigned int startIndex = 
      9 * config::MAX_N_SPECIES * config::MAX_N_BOIDS * sizeof(float);

   for (int i = 0; i < config::MAX_N_OBSTACLES; i++)
   {
      for (int j = 0; j < config::N_TRIANG_PER_CIRCLE; j++)
      {
         const unsigned int vertexOffset =
            (
             (9 * j) +
             (i * config::N_TRIANG_PER_CIRCLE * 9)
            ) * sizeof(float) + startIndex;

         updateChunkOfBuffer(
               bufferColor,
               vertexOffset,
               9,
               &obstacles[i].color
         );
      }
   }
}

// fix this!

void Simulation::fillObstacleChunk()
{
   const unsigned int startIndex =
      6 * config::MAX_N_BOIDS * config::MAX_N_SPECIES * sizeof(float);

   for (int i = 0; i < config::MAX_N_OBSTACLES; i++)
   {
      for (int j = 0; j < config::N_TRIANG_PER_CIRCLE; j++)
      {
         const unsigned int vertexOffset =
            (
             (6 * j) +
             (i * config::N_TRIANG_PER_CIRCLE * 6)
            ) * sizeof(float) + startIndex;

         updateChunkOfBuffer(
               bufferVertex,
               vertexOffset,
               6,
               &obstacles[i].pos[j]
         );
      }
   }
}

// ========================== ImGui functions ==========================

void Simulation::initImGui()
{
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO(); (void) io;

   ImGui::StyleColorsDark();
   
   ImGui_ImplSDL2_InitForOpenGL(window->getWindow(), glContext);
   ImGui_ImplOpenGL3_Init();
}

void Simulation::updateGUI()
{
   // Start the Dear ImGui frame
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplSDL2_NewFrame(window->getWindow());
   ImGui::NewFrame();

   // Draw GUI
   ImGui::Begin("Settings");
   ImGui::SliderFloat("Alignment" , &config::alignmentScalar, 0.0f, 2.0f);
   ImGui::SliderFloat("Cohesion"  , &config::cohesionScalar, 0.0f, 2.0f);
   ImGui::SliderFloat("Separation", &config::separationScalar, 0.0f, 2.0f);

   ImGui::End();
}
