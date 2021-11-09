#include <iostream>
#include <exception>
#include <string.h>
#include <vector>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <boidSimulation/WindowManager.h>
#include <boidSimulation/Renderer.h>
//#include <boidSimulation/ResourceManager.h>
#include <boidSimulation/glShaderLoader.h>

class Bird
{

public:
   float pos[9];

   Bird(float x, float y)
   : pos{ -0.4f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f}
   {}
      //x - 0.01f, y, 0.0f,
      //x + 0.01f, y, 0.0f,
      //x, y + 0.1f, 0.0f }

   static size_t getSizeOfPosArray()
   {
      return sizeof(pos) / sizeof(float);
   }

   ~Bird() = default;
private:
};

std::vector<Bird> allBirds;

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

void generateNewBird(float x, float y, GLuint VAO)
{
   std::cout << x << " " << y << std::endl;
   Bird newBird(x,y);
   allBirds.push_back(newBird);

   GLuint VBO;
   glGenBuffers(1, &VBO);
   
   glBindVertexArray(VAO);
   
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(
         GL_ARRAY_BUFFER, 
         newBird.getSizeOfPosArray(),
         newBird.pos,
         GL_STATIC_DRAW
   );

   glVertexAttribPointer(
         0, 3, 
         GL_FLOAT, 
         GL_FALSE,
         3 * sizeof(float), (void*)0
   );
   glEnableVertexAttribArray(0);

   //glBindBuffer(GL_ARRAY_BUFFER, 0);
   //glBindVertexArray(0);

}

void update()
{
   createGUI();

}

void render(GLuint& shaderProgram, GLuint& VAO)
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
   glDrawArrays(GL_TRIANGLES, 0, 3);
}

// We load all the resources of the app
bool load(GLuint& shaderProgram, GLuint& VAO)
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
   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);

   glBindVertexArray(VAO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(birdPos), birdPos, GL_STATIC_DRAW);
   
   // Now we need to specify of how OpenGl has to
   // interpret the vertex data that we sent to the GPU.
   // 1) Vertex attribute location(e.h layout (location = 0) in v.s).
   // 2) Size of each vertex(if it's a 3d vertex -> 3 coord.).
   // 3) Data Type.
   // 4) If we want to normalize.
   // 5)Stride -> space between consecutive vertex attributes.
   // 6) Position where data begins in the buffer.
   //
   // Also with this we link the vertex data(from the VBO) with
   // the vertex shader's vertex attributes.
   // ### A vertex attribute is an input variable to a shader (vertex shader).
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   //Now we can unbind(this is to leave it free for the next bind)
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);

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
   GLuint shaderProgram, VAO;
   if (load(shaderProgram, VAO) == false)
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
         int x, y;
         if (event.type == SDL_MOUSEBUTTONDOWN && SDL_GetMouseState(&x, &y) == SDL_BUTTON(1)) {
            generateNewBird(x / 680.0f, y / 480.0f, VAO);
         }
                  
      }

      if (!appRunning)
         break;

      
      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL2_NewFrame(window->getWindow());
      ImGui::NewFrame();

      update();

      render(shaderProgram, VAO);
      
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
