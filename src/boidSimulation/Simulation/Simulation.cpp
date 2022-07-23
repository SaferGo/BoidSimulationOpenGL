#include <boidSimulation/Simulation.h>

#include <exception>

Simulation::Simulation()
{
   
   try 
   {
      // Init Window
      m_window = WindowManager::getInstance(
            config::RESOLUTION_W,
            config::RESOLUTION_H,
            config::WINDOW_TITLE
      );

      // Init Renderer
      m_renderer = Renderer::getInstance(m_window->getWindow());

      initGLAD();

      m_glContext = SDL_GL_CreateContext(m_window->getWindow());

      initImGui();

   } catch (const std::exception& e)
   {
      std::cout << e.what() << std::endl;
      m_appRunning = false;

      return;
   }

   createFlocks();
   createObstacles();

   initShaders();
   initBuffers();
   
   m_appRunning = true;
   m_FPS = 0;
}

Simulation::~Simulation()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext();

   SDL_GL_DeleteContext(m_glContext);
   m_renderer->destroyRenderer();
   m_window->destroyWindow();
}

// Creates n species of boids.
void Simulation::createFlocks()
{
   m_flocks.reserve(config::MAX_N_SPECIES);
  
   for (int i = 0; i < config::MAX_N_SPECIES; i++)
      m_flocks.push_back(Flock());
}

void Simulation::createObstacles()
{
   m_obstacles.reserve(config::MAX_N_OBSTACLES);

   for (int i = 0; i < config::MAX_N_OBSTACLES; i++)
   {
      m_obstacles.push_back(Obstacle(i));
      
      while (m_obstacles[i].doesCollide(m_obstacles) == true)
         m_obstacles[i] = Obstacle(i);

   }
}

void Simulation::calculateFPS()
{
   static int fps = 0;
   static float lastTime = 0.0f;

   float currentTime = SDL_GetTicks() * 0.001f;
   fps++;

   if (currentTime - lastTime > 1.0f)
   {
      m_FPS = fps;
      lastTime = currentTime;
      fps = 0;
   }
}

void Simulation::run()
{
   while (m_appRunning) {

      update();
      render();

      calculateFPS();
   }
}


void Simulation::update()
{
   checkUserInput();

   updateGUI();

   updateFlocks();
   
   updateBoidChunk();
}

void Simulation::updateFlocks()
{
   for (auto& flock : m_flocks)
      flock.toFlock(m_obstacles);
}

void Simulation::render()
{
   // Renders the frame logic
   glClearColor(0.4, 0.1, 0.2, 1);
   glClear(GL_COLOR_BUFFER_BIT);

   
   // Renders the triangles
   glUseProgram(m_shaderProgram);
   glBindVertexArray(m_VAO);
   glDrawArrays(
         GL_TRIANGLES,
         0,
         (
            (3 * config::MAX_N_BOIDS * config::MAX_N_SPECIES) +
            (3 * config::MAX_N_OBSTACLES * config::N_TRIANG_PER_CIRCLE)
         )
   );
   
   glBindVertexArray(m_VAO);

   // Renders imgui
   // (we've to render it after glDraw to appear on top of the _window)
   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

   m_window->swapBuffers();
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
             event.window.windowID == SDL_GetWindowID(m_window->getWindow())
         )
      ){
         m_appRunning = false;
         break;
      }
   }
}

// ========================== OpenGL functions ==========================

// Initializes GLAD and load all the GL function pointers.
void Simulation::initGLAD()
{
   if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
      throw std::runtime_error("Failed to initialize GLAD.");
}

// Updates the chunk of the buffer that contains the boids
void Simulation::updateBoidChunk()
{
   for (int i = 0; i < config::MAX_N_SPECIES; i++)
   {
      for (int j = 0; j < config::MAX_N_BOIDS; j++)
      {
         unsigned int vertexOffset = (
               (6 * j) +
               (i * config::MAX_N_BOIDS * 6)
         ) * sizeof(float);

         updateChunkOfBuffer(
               m_bufferVertex, 
               vertexOffset,
               6,
               m_flocks[i].getBoidPosition(j)
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
   glBindVertexArray(m_VAO);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   
   glBufferSubData(
         GL_ARRAY_BUFFER, vertexOffset, sizeof(float) * sizeData, data
   );

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

// Builds and compiles the shaders.
void Simulation::initShaders()
{
   try
   {
      m_shaderProgram =
         glShaderLoader::loadShader(
            "../shaders/boids.vert", 
            "../shaders/boids.frag"
      );
   } catch(const std::exception& e)
   {
      std::cerr << e.what() << std::endl;
      m_appRunning = false;
   }
}

void Simulation::initBuffers()
{
   glGenVertexArrays(1, &m_VAO);
   
   createVertexBuffer();
   createColorBuffer();
   
   // Fills static buffers
   fillObstacleChunk();
   fillColorBoidBuffer();
}

void Simulation::createBuffer(
      const size_t bufferSize,
      unsigned int& buffer,
      const GLenum bufferType,
      const GLenum drawingType
) {

   glGenBuffers(1, &buffer);
   // We indicate the type of buffer
   glBindBuffer(bufferType, buffer);
   // We allocate the buffer with determined size but with 0 elements.
   glBufferData(bufferType, bufferSize, NULL, drawingType);

   // To unbind the buffer(state-machine..)
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Simulation::bindBufferToVAO(
      const unsigned int vao,
      const unsigned int index,
      const unsigned int buffer,
      const unsigned int offset,
      const unsigned int stride
) {
   // We bind the VAO to configure it.
   glBindVertexArray(vao);

   // 1) VAO.
   // 2) VAO's index that we want to use for this buffer.
   // (vertex buffer binding index)
   // (VAO contains "the pointers" to the VBOs.
   glEnableVertexArrayAttrib(vao, index);

   // 1) VAO,
   // 2) Vertex buffer binding index
   // 3) VBO
   // 4) Offset within the VBO.
   // 5) Stride(distance between the elements within the VBO).
   glVertexArrayVertexBuffer(vao, index, buffer, offset, stride);

   // After we finished all the configuration, we need to unbind the VAO to
   // prevent future errors.
   glBindVertexArray(0);
}

// Creates the "binding" with the shader.
void Simulation::bindVertexAttribute(
      const unsigned int vao,
      const unsigned int vaoIndex,
      const unsigned int shaderAttribIndex,
      const unsigned int nComponents,
      const GLenum type,
      const GLenum isNormalized,
      const unsigned int distance
) {

   glBindVertexArray(vao);

   // Associates a vertex attribute(an input variable to a shader that is 
   // supplied with per-vertex data) and a vertex buffer binding for a vertex
   // array object.
   // 1) VAO
   // 2) Shader attribute index(the one that is in the shader).
   // 3) Vertex buffer binding index.

   glVertexArrayAttribBinding(vao, shaderAttribIndex, vaoIndex);

   // 1) VAO.
   // 2) Shader Attribute index.
   // 3) Number of components.
   // 4) Type of component.
   // 5) If the components are normalized.
   // 6) Distance between elements within the buffer.
   glVertexArrayAttribFormat(
         vao, shaderAttribIndex, nComponents, type, isNormalized, distance
   );

   glBindVertexArray(0);
}


// Creates the buffer that will contains all the vertices of the boids and
// the obstacles.
void Simulation::createVertexBuffer()
{
   // Create buffer
   size_t bufferSize = (
       (6 * config::MAX_N_BOIDS * config::MAX_N_SPECIES) +
       (6 * config::N_TRIANG_PER_CIRCLE * config::MAX_N_OBSTACLES)
   ) * sizeof(float);

   // Create a big empty buffer for all the boids and the obstacles.
   createBuffer(bufferSize, m_bufferVertex, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
   bindBufferToVAO(m_VAO, 0, m_bufferVertex, 0, sizeof(float) * 2);
   bindVertexAttribute(m_VAO, 0, 0, 2, GL_FLOAT, GL_FALSE, 0);
}

// Creates the buffer that will contains all the colors for all the boids
// and obstacles.
void Simulation::createColorBuffer()
{
   size_t bufferSize = (
         (9 * config::MAX_N_BOIDS * config::MAX_N_SPECIES) +
         (9 * config::N_TRIANG_PER_CIRCLE * config::MAX_N_OBSTACLES)
   ) * sizeof(float);

   createBuffer(bufferSize, m_bufferColor, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
   bindBufferToVAO(m_VAO, 1, m_bufferColor, 0, sizeof(float) * 3);
   bindVertexAttribute(m_VAO, 1, 1, 3, GL_FLOAT, GL_FLOAT, 0);
}

// Fills the chunk that belongs to the boid's color.
void Simulation::fillColorBoidBuffer()
{
   for (int i = 0; i < config::MAX_N_SPECIES; i++)
   {
      for (int j = 0; j < config::MAX_N_BOIDS; j++)
      {
         const unsigned int vertexOffset = (
             (9 * j) +
             (i * config::MAX_N_BOIDS * 9)
         ) * sizeof(float);

         updateChunkOfBuffer(
               m_bufferColor,
               vertexOffset,
               9,
               m_flocks[i].getColor()
         );
      }
   }
}

// Fills the color and vertex chunks of the obstacles.
void Simulation::fillObstacleChunk()
{
   const unsigned int startIndexVertex =
      6 * config::MAX_N_BOIDS * config::MAX_N_SPECIES * sizeof(float);
   const unsigned int startIndexColor = 
      9 * config::MAX_N_SPECIES * config::MAX_N_BOIDS * sizeof(float);


   for (int i = 0; i < config::MAX_N_OBSTACLES; i++)
   {
      for (int j = 0; j < config::N_TRIANG_PER_CIRCLE; j++)
      {

         // Fills the vertex buffer

         const unsigned int vertexOffset = (
             (6 * j) +
             (i * config::N_TRIANG_PER_CIRCLE * 6)
         ) * sizeof(float) + startIndexVertex;

         updateChunkOfBuffer(
               m_bufferVertex,
               vertexOffset,
               6,
               m_obstacles[i].getPos(j)
         );

         // Fills the color buffer

         const unsigned int colorOffset = (
             (9 * j) +
             (i * config::N_TRIANG_PER_CIRCLE * 9)
         ) * sizeof(float) + startIndexColor;
         
         updateChunkOfBuffer(
               m_bufferColor,
               colorOffset,
               9,
               m_obstacles[i].getColor()
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
   
   ImGui_ImplSDL2_InitForOpenGL(m_window->getWindow(), m_glContext);
   ImGui_ImplOpenGL3_Init();
}

void Simulation::updateGUI()
{
   // Starts the Dear ImGui frame
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplSDL2_NewFrame(m_window->getWindow());
   ImGui::NewFrame();

   // Draws the GUI
   ImGui::Begin("Settings");
   ImGui::Text("FPS: %s", std::to_string(m_FPS).c_str());
   ImGui::Text(
         "Number of species: %s", std::to_string(config::MAX_N_SPECIES).c_str()
   );
   ImGui::Text(
         "Number of boids per specie: %s",
         std::to_string(config::MAX_N_BOIDS).c_str()
   );
   ImGui::Text(
         "Number of obstacles: %s",
         std::to_string(config::MAX_N_OBSTACLES).c_str()
   );
   ImGui::SliderFloat("Alignment" , &config::alignmentScalar, 0.0f, 2.0f);
   ImGui::SliderFloat("Cohesion"  , &config::cohesionScalar, 0.0f, 2.0f);
   ImGui::SliderFloat("Separation", &config::separationScalar, 0.0f, 2.0f);

   ImGui::End();
}
