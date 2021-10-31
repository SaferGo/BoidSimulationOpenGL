#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <glShaderLoader.h>

#include <iostream>
#include <exception>
#include <string.h>


void initGLAD()
{
   // Here we initialize GLAD and loads all the GL function pointers.
   if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
      throw std::runtime_error("Failed to initialize GLAD.");
}

void initSDL()
{
   if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      throw std::runtime_error(
               strcat(
                  (char*)"Failed to initialize the SDL2 library.\nError: ",
                  SDL_GetError()
               )
      );
   }
}

void initWindow(SDL_Window*& window)
{
   window = SDL_CreateWindow(
         "Boid Simulation",
         SDL_WINDOWPOS_CENTERED,
         SDL_WINDOWPOS_CENTERED,
         680,
         480,
         SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
   );

   if (!window) {
      throw std::runtime_error(
               strcat(
                  (char*)"Failed to create SDL2 window.\nError: ",
                  SDL_GetError()
               )
      );
   }
}

void initRenderer(SDL_Renderer*& renderer, SDL_Window*& window)
{
   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

   if (!renderer) {
      throw std::runtime_error(
               strcat(
                  (char*)"Failed to create SDL2 renderer.\nError: ",
                  SDL_GetError()
               )
      );
   }
}

void initImGui(SDL_Window*& window, SDL_GLContext& context)
{
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO(); (void) io;

   ImGui::StyleColorsDark();
   
   ImGui_ImplSDL2_InitForOpenGL(window, context);
   ImGui_ImplOpenGL3_Init();
}

int main() 
{

   SDL_Window* window;
   SDL_Renderer* renderer;

   try {
      initSDL();
      initWindow(window);
      initRenderer(renderer, window);
      initGLAD();
   } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;

      return -1;
   }
   
   SDL_GLContext glContext = SDL_GL_CreateContext(window);
   
   try {
      initImGui(window, glContext);
   } catch (const std::exception& e){
      std::cerr << e.what() << std::endl;

      return -1;
   }


   while (running) {

      // Get input
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
         ImGui_ImplSDL2_ProcessEvent(&event);
         if (event.type == SDL_QUIT ||
            (event.type == SDL_WINDOWEVENT &&
             event.window.event == SDL_WINDOWEVENT_CLOSE &&
             event.window.windowID == SDL_GetWindowID(window))) {
            running = false;
         }
      }

      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL2_NewFrame(window);
      ImGui::NewFrame();

      // Frame logic here
      glClearColor(0.4, 0.1, 0.2, 1);
      glClear(GL_COLOR_BUFFER_BIT);

      // Render other stuff
      
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

      unsigned int VBO;
      glGenBuffers(1, &VBO);
      // Set the type of VBO
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      // Send the data to the buffer
      glBufferData(GL_ARRAY_BUFFER, sizeof(birdPos), birdPos, GL_STATIC_DRAW);

      GLuint shaderProgram;
      // Now we'll create the vertex and fragment shader
      try {
         shaderProgram = loadShader("birds.vert", "birds.frag");
      } catch(const std::exception& e) {
         std::cerr << e.what() << std::endl;
         
         return -1;
      }

      glUseProgram(shaderProgram);



      // ...

      // Create ImGui windows
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
      

      // Render imgui
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // Updates the window with OpenGL rendering
      SDL_GL_SwapWindow(window);
   }

   // Delete/destroy everything
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext();

   SDL_GL_DeleteContext(glContext);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);

	return 0;
}
