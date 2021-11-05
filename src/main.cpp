#include <iostream>
#include <exception>
#include <string.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <boidSimulation/WindowManager.h>
#include <boidSimulation/Renderer.h>
//#include <boidSimulation/ResourceManager.h>
#include <boidSimulation/glShaderLoader.h>

void initGLAD()
{
   // Here we initialize GLAD and load all the GL function pointers.
   if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
      throw std::runtime_error("Failed to initialize GLAD.");
}



void initImGui(SDL_Window* window, SDL_GLContext& context)
{
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO(); (void) io;

   ImGui::StyleColorsDark();
   
   ImGui_ImplSDL2_InitForOpenGL(window, context);
   ImGui_ImplOpenGL3_Init();
}

void createGUI()
{
   ImGui::Begin("MyWindow");
   bool pepe;
   float speed;
   ImGui::Checkbox("Boolean propery", &pepe);
   if (ImGui::Button("Reset Speed")) {
      speed = 0;
   }
   ImGui::SliderFloat("Speed", &speed, 0.0f, 10.0f);
   ImGui::End();
}

void update()
{
   createGUI();
}

void render()
{
   // Render imgui
   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// We load all the resources of the app
bool load(GLuint& shaderProgram)
{
   try {
      shaderProgram =
         glShaderLoader::loadShader(
            "../shaders/birds.vert", 
            "../shaders/birds.frag"
      );
   } catch(const std::exception& e) {
      std::cerr << e.what() << std::endl;
         
      return false;
   }

   glUseProgram(shaderProgram);

   float birdPos[] = {
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      0.0f,  0.5f, 0.0f
   };

      // To send this data to the GPU
      // first we'll create a VBO 
      // to set a memory buffer in the GPU
      // **Remember that to send data from
      // the CPU to the GPU is very slow, so
      // is better to send as much data as
      // possible at once.

      /*unsigned int VBO;
      glGenBuffers(1, &VBO);
      // Set the type of VBO
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      // Send the data to the buffer
      glBufferData(GL_ARRAY_BUFFER, sizeof(birdPos), birdPos, GL_STATIC_DRAW);
      */
   return true;
}

int main() 
{
   WindowManager* window;
   Renderer* renderer;
   SDL_GLContext glContext;

   try {
      window = WindowManager::getInstance(680, 480, "boid simulator");
      renderer = Renderer::getInstance(window->getWindow());

      initGLAD();
      glContext = SDL_GL_CreateContext(window->getWindow());
      initImGui(window->getWindow(), glContext);
   } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
      
      return -1;
   }

   // Here we'll load all the resources of our app
   GLuint shaderProgram;
   if (load(shaderProgram) == false)
      return -1;

   bool appRunning = true;

   while (appRunning) {

      // Get the input
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
         ImGui_ImplSDL2_ProcessEvent(&event);

         if (event.type == SDL_QUIT ||
            (event.type == SDL_WINDOWEVENT &&
               event.window.event == SDL_WINDOWEVENT_CLOSE &&
               event.window.windowID == SDL_GetWindowID(window->getWindow()))) {
            appRunning = false;
            break;
         }
      }

      if (!appRunning)
         break;

      
      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL2_NewFrame(window->getWindow());
      ImGui::NewFrame();

      // Frame logic here
      glClearColor(0.4, 0.1, 0.2, 1);
      glClear(GL_COLOR_BUFFER_BIT);

      // Render other stuff
      
      update();

      render();
      
      window->swapBuffers();
   }

   // Destroy everything
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext();

   SDL_GL_DeleteContext(glContext);
   renderer->destroyRenderer();
   window->destroyWindow();

	return 0;
}
