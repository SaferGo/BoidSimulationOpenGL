#include <boidSimulation/Simulation.h>

#include <exception>

Simulation::Simulation(const int nSpecies, const int nBoids)
{
   init();

   // Create n speacies of boids
   for (int i = 0; i < nSpecies; i++)
   {
      Flock newFlock = Flock(nBoids);
      flocks.push_back(newFlock);
   }

   updateAllBuffer();
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
   setupVertices();
}

void Simulation::update()
{
   updateGUI();

   for (auto& flock : flocks)
      flock.toFlock();
   
   updateAllBuffer();
}

void Simulation::render()
{
   // Render frame logic
   glClearColor(0.4, 0.1, 0.2, 1);
   glClear(GL_COLOR_BUFFER_BIT);

   // Render imgui
   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

   // Render stuff
   glUseProgram(shaderProgram);
   glBindVertexArray(VAO);
   glDrawArrays(GL_TRIANGLES, 0, 3 * config::MAX_N_BOIDS);

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
            event.window.windowID == SDL_GetWindowID(window->getWindow())))
      {
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

void Simulation::updateAllBuffer()
{
   for (int i = 0; i < config::MAX_N_BOIDS; i++)
      updateChunkOfBuffer(i, flocks[0].boids[i].pos);
}

void Simulation::updateChunkOfBuffer(const int i, glm::vec2* data)
{
   glBindVertexArray(VAO);

   const unsigned int vertexOffset = 6 * i * sizeof(float); 
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferSubData(
         GL_ARRAY_BUFFER, vertexOffset, sizeof(float) * 2 * 3, data
   );

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

void Simulation::initShaders()
{
   // Build and compile the shaders.
   try
   {
      shaderProgram =
         glShaderLoader::loadShader(
            "../shaders/birds.vert", 
            "../shaders/birds.frag"
      );
   } catch(const std::exception& e)
   {
      std::cerr << e.what() << std::endl;
      appRunning = false;

      return;
   }

}

void Simulation::setupVertices()
{
   glGenVertexArrays(1, &VAO);
   glBindVertexArray(VAO);

   size_t vertexBufferSize = 6 * sizeof(float) * config::MAX_N_BOIDS;

   // Create a big buffer for all the boids
   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   // We allocate the buffer with determined size but with 0 elements.
   glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_DYNAMIC_DRAW);
   
   // Position attribute
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
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


