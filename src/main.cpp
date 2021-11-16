#include <iostream>
#include <exception>
#include <string.h>
#include <vector>
#include <random>
#include <ctime>
#include <cmath>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <boidSimulation/WindowManager.h>
#include <boidSimulation/Renderer.h>
//#include <boidSimulation/ResourceManager.h>
#include <boidSimulation/glShaderLoader.h>

#define PI 3.1415
std::mt19937 gen(time(0));
const int MAX_N_BOIDS = 10;
int boidsIn = 0;
GLuint buffer, VAO;
unsigned int startTime, currentTime;
double deltaTime;

struct Pos
{
   float x, y;

   Pos() {}
   Pos(const float newX, const float newY) : x(newX), y(newY) {}
   Pos(const Pos& newPos) : x(newPos.x), y(newPos.y) {}
};

class Bird
{
public:
   float pos[9];
   Pos center;
   float directionalAngle;

   Bird(float x, float y)
   : pos{ x - 0.02f, y, 0.0f, x + 0.02f, y, 0.0f, x, y + 0.1f, 0.0f},
     center(x, y)
   {}

   static size_t getSizeOfPosArray()
   {
      return sizeof(pos) / sizeof(float);
   }

   void rotate()
   {
      for (int i = 0; i <= 6; i += 3) {
         float x1 = pos[i] - center.x;
         float y1 = pos[i + 1] - center.y;

         float x2 = 
            x1 * cos(directionalAngle) - 
            y1 * sin(directionalAngle);
         float y2 =
            x1 * sin(directionalAngle) +
            y1 * cos(directionalAngle);

         pos[i] = x2 + center.x;
         pos[i + 1] = y2 + center.y;
      }
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

void updateVerticesBird(const int bird)
{
   Pos centerBird = allBirds[bird].center;
   allBirds[bird] = Bird(centerBird.x, centerBird.y);
}

void generateNewBird(float x, float y)
{
   if (boidsIn > MAX_N_BOIDS)
      return;

   Bird newBird(x, y);
   
   std::uniform_int_distribution<> randAngle(0, 360);
   float newDirection = randAngle(gen) * PI / 180.0;
   newBird.directionalAngle = newDirection;
   newBird.rotate();

   glBindVertexArray(VAO);

   const unsigned int vertexOffset = 9 * boidsIn * sizeof(float); 
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferSubData(
         GL_ARRAY_BUFFER, vertexOffset, sizeof(newBird.pos), newBird.pos
   );

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
   
   allBirds.push_back(newBird);
   boidsIn++;
}

void updateBuffer(const int bird)
{
   const int vertexOffset = 9 * bird * sizeof(float);

   glBindVertexArray(VAO);

   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferSubData(
         GL_ARRAY_BUFFER,
         vertexOffset,
         sizeof(allBirds[bird].pos),
         allBirds[bird].pos
   );
   
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

void randomFly()
{
   for (int i = 0; i < boidsIn; i++) {

      const float step = 0.01;
      const float speed = 0.008;
      double newDirection;

      // The birds rotate after a period of time
      bool rotate = false;
      currentTime = SDL_GetPerformanceCounter();
      deltaTime = (double) (
         (currentTime - startTime) /
         (double) SDL_GetPerformanceFrequency()
      );

      std::cout << "DELTA: " << deltaTime << std::endl;
      if (deltaTime > 0.001) {
         rotate = true;

         std::uniform_int_distribution<> randAngle(-7, 7);

         // Since the triangles are already 90 degrees rotated,
         // we need to add it to the translation
         newDirection = double (
            (randAngle(gen) * PI / 180.0) +
            allBirds[i].directionalAngle +
            1.5708
         );

         newDirection = std::fmod(newDirection, PI * 2.0);

                  
         startTime = SDL_GetPerformanceCounter();
      } else {
         // If the bird doesn't rotate, we just move foward
         newDirection = allBirds[i].directionalAngle + 1.5708;
      }

      allBirds[i].center.x += cos(newDirection) * step * speed;
      allBirds[i].center.y += sin(newDirection) * step * speed;


      if (allBirds[i].center.x > 1.0)
         allBirds[i].center.x = -1.0 + std::fmod(allBirds[i].center.x, 1.0);
      
      if (allBirds[i].center.x < -1.0)
         allBirds[i].center.x = 1.0 - std::fmod(allBirds[i].center.x, 1.0);

      if (allBirds[i].center.y > 1.0)
         allBirds[i].center.y = -1.0 + std::fmod(allBirds[i].center.y, 1.0);
      
      if (allBirds[i].center.y < -1.0)
         allBirds[i].center.y = 1.0 - std::fmod(allBirds[i].center.y, 1.0);

      
      updateVerticesBird(i);

      if (1) {
         newDirection -= 1.5708;

         allBirds[i].directionalAngle = newDirection;
         allBirds[i].rotate();
      }

      updateBuffer(i);
   }
}

void update()
{
   createGUI();

   randomFly();
}

void render(GLuint& shaderProgram)
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
   glDrawArrays(GL_TRIANGLES, 0, 3 * boidsIn);

   glBindVertexArray(VAO);
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

   glGenVertexArrays(1, &VAO);
   glBindVertexArray(VAO);
   // Create big buffer for all the boids

   size_t vertexBufferSize = 9 * sizeof(float) * MAX_N_BOIDS;

   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   // We allocate the buffer with determined size but with 0 elements.
   glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_DYNAMIC_DRAW);
   
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);



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
   GLuint shaderProgram;
   if (load(shaderProgram) == false)
      return -1;

   startTime = SDL_GetPerformanceCounter();

   bool appRunning = true;

   while (appRunning) {

      // Get the input
      SDL_Event event;
      // If we use SDL_PollEvent
      // is when we have a game
      // full of animation, we need 
      // precision in our input.
      // And we use WaitEvent when
      // the rendering of the game
      // depends of the next
      // input(like a chess game).
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
            float nx = (x / 680.0f) - 0.5f;
            float ny = (y / 480.0f) - 1.0f;
            ny *= -1;
            ny -= 1.0f;
            generateNewBird(nx, ny);
         }
                  
      }

      if (!appRunning)
         break;

      
      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL2_NewFrame(window->getWindow());
      ImGui::NewFrame();

      update();

      render(shaderProgram);
      
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
