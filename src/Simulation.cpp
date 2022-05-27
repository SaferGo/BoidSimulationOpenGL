#include <boidSimulation/Simulation.h>

#include <exception>

Simulation::Simulation()
{
   
   try 
   {
      // Init Window
      _window = WindowManager::getInstance(
            config::RESOLUTION_W,
            config::RESOLUTION_H,
            config::WINDOW_TITLE
      );

      // Init Renderer
      _renderer = Renderer::getInstance(_window->getWindow());

      initGLAD();

      _glContext = SDL_GL_CreateContext(_window->getWindow());

      initImGui();

   } catch (const std::exception& e)
   {
      std::cout << e.what() << std::endl;
      _appRunning = false;

      return;
   }

   createFlocks();
   createObstacles();

   initShaders();
   initBuffers();
   
   _appRunning = true;
   _FPS = 0;
}

Simulation::~Simulation()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext();

   SDL_GL_DeleteContext(_glContext);
   _renderer->destroyRenderer();
   _window->destroyWindow();
}

void Simulation::createFlocks()
{
   // Create n species of boids

   _flocks.reserve(config::MAX_N_SPECIES);
  
   for (int i = 0; i < config::MAX_N_SPECIES; i++)
      _flocks.push_back(Flock());
}

void Simulation::createObstacles()
{
   _obstacles.reserve(config::MAX_N_OBSTACLES);

   for (int i = 0; i < config::MAX_N_OBSTACLES; i++)
   {
      _obstacles.push_back(Obstacle(i));
      
      while (_obstacles[i].doesCollide(_obstacles) == true)
         _obstacles[i] = Obstacle(i);

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
      _FPS = fps;
      lastTime = currentTime;
      fps = 0;
   }
}

void Simulation::run()
{
   while (_appRunning) {

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
   for (auto& flock : _flocks)
      flock.toFlock(_obstacles);
}

void Simulation::render()
{
   // Render frame logic
   glClearColor(0.4, 0.1, 0.2, 1);
   glClear(GL_COLOR_BUFFER_BIT);

   
   // Render triangles
   glUseProgram(_shaderProgram);
   glBindVertexArray(_VAO);
   glDrawArrays(
         GL_TRIANGLES,
         0,
         (
            (3 * config::MAX_N_BOIDS * config::MAX_N_SPECIES) +
            (3 * config::MAX_N_OBSTACLES * config::N_TRIANG_PER_CIRCLE)
         )
   );
   
   glBindVertexArray(_VAO);

   // Render imgui
   // (we've to render it after glDraw to appear on top of the _window)
   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

   _window->swapBuffers();
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
             event.window.windowID == SDL_GetWindowID(_window->getWindow())
         )
      ){
         _appRunning = false;
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
         unsigned int vertexOffset = (
               (6 * j) +
               (i * config::MAX_N_BOIDS * 6)
         ) * sizeof(float);

         updateChunkOfBuffer(
               _bufferVertex, 
               vertexOffset,
               6,
               _flocks[i].getBoidPosition(j)
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
   glBindVertexArray(_VAO);
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
      _shaderProgram =
         glShaderLoader::loadShader(
            "../shaders/boids.vert", 
            "../shaders/boids.frag"
      );
   } catch(const std::exception& e)
   {
      std::cerr << e.what() << std::endl;
      _appRunning = false;
   }
}

void Simulation::initBuffers()
{
   glGenVertexArrays(1, &_VAO);
   
   createVertexBuffer();
   createColorBuffer();
   
   // Fill static buffers
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
   // We bind the VAO to configure it
   glBindVertexArray(vao);

   // Seleccionamos que index del _VAO queremos usar para este buffer.
   // Esto se llama "vertex buffer binding index" -> indice usado en el _VAO para
   // identificar un VBO.
   // (en este caso queremos usar el 0)
   // (acordate que el _VAO contine los "punteros" a los VBO)
   glEnableVertexArrayAttrib(vao, index);
   // 1) _VAO,
   // 2) Index of VBO in _VAO,
   // 3) VBO,
   // 4) Offset within the VBO,
   // 5) stride(distance between the elements within the VBO).
   glVertexArrayVertexBuffer(vao, index, buffer, offset, stride);

   glBindVertexArray(0);
}

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

   // Aca hacemos la conexion con el shader.
   // Associates a vertex attribute(an input variable to a shader that is 
   // supplied with per-vertex data) and a vertex buffer binding for a vertex
   // array object.
   // 1) _VAO
   // 2) Shader attribute index(el que esta en el shader).
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


void Simulation::createVertexBuffer()
{
   // Create buffer
   size_t bufferSize = (
       (6 * config::MAX_N_BOIDS * config::MAX_N_SPECIES) +
       (6 * config::N_TRIANG_PER_CIRCLE * config::MAX_N_OBSTACLES)
   ) * sizeof(float);

   // Create a big buffer for all the boids and the obstacles
   createBuffer(bufferSize, _bufferVertex, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
   bindBufferToVAO(_VAO, 0, _bufferVertex, 0, sizeof(float) * 2);
   bindVertexAttribute(_VAO, 0, 0, 2, GL_FLOAT, GL_FALSE, 0);
}

void Simulation::createColorBuffer()
{
   size_t bufferSize = (
         (9 * config::MAX_N_BOIDS * config::MAX_N_SPECIES) +
         (9 * config::N_TRIANG_PER_CIRCLE * config::MAX_N_OBSTACLES)
   ) * sizeof(float);

   createBuffer(bufferSize, _bufferColor, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
   bindBufferToVAO(_VAO, 1, _bufferColor, 0, sizeof(float) * 3);
   bindVertexAttribute(_VAO, 1, 1, 3, GL_FLOAT, GL_FLOAT, 0);
}

void Simulation::fillColorBoidBuffer()
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
               _bufferColor,
               vertexOffset,
               9,
               _flocks[i].getColor()
         );
      }
   }
}

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

         // Fill vertex buffer

         const unsigned int vertexOffset = (
             (6 * j) +
             (i * config::N_TRIANG_PER_CIRCLE * 6)
         ) * sizeof(float) + startIndexVertex;

         updateChunkOfBuffer(
               _bufferVertex,
               vertexOffset,
               6,
               _obstacles[i].getPos(j)
         );

         // Fill color buffer

         const unsigned int colorOffset = (
             (9 * j) +
             (i * config::N_TRIANG_PER_CIRCLE * 9)
         ) * sizeof(float) + startIndexColor;
         
         updateChunkOfBuffer(
               _bufferColor,
               colorOffset,
               9,
               _obstacles[i].getColor()
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
   
   ImGui_ImplSDL2_InitForOpenGL(_window->getWindow(), _glContext);
   ImGui_ImplOpenGL3_Init();
}

void Simulation::updateGUI()
{
   // Start the Dear ImGui frame
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplSDL2_NewFrame(_window->getWindow());
   ImGui::NewFrame();

   // Draw GUI
   ImGui::Begin("Settings");
   ImGui::Text("_FPS: %s", std::to_string(_FPS).c_str());
   ImGui::SliderFloat("Alignment" , &config::alignmentScalar, 0.0f, 2.0f);
   ImGui::SliderFloat("Cohesion"  , &config::cohesionScalar, 0.0f, 2.0f);
   ImGui::SliderFloat("Separation", &config::separationScalar, 0.0f, 2.0f);

   ImGui::End();
}
