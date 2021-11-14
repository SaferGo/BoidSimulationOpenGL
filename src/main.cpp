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

std::mt19937 gen(time(0));
const int MAX_N_BOIDS = 10;
int boidsIn = 0;
GLuint buffer, VAO;

struct Pos
{
   float x, y;

   Pos() {}
   Pos(const float newX, const float newY) : x(newX), y(newY) {}
   Pos(const Pos& newPos) : x(newPos.x), y(newPos.y) {}
};

struct CurrentPathInfo
{
   Pos start, end, curve;
   float t;

   CurrentPathInfo(const Pos newStart, const Pos newEnd, const Pos newCurve)
      : start(newStart), end(newEnd), curve(newCurve), t(0.0) {}
};

class Bird
{
public:
   float pos[9];
   Pos center;

   Bird(float x, float y)
   : pos{ x - 0.02f, y, 0.0f, x + 0.02f, y, 0.0f, x, y + 0.1f, 0.0f},
     center(x, y)
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
std::vector<bool> isFlying;
std::vector<Pos> actualTarget;
std::vector<CurrentPathInfo> currentPath;

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

void generateNewBird(float x, float y)
{
   if (boidsIn > MAX_N_BOIDS)
      return;

   Bird newBird(x, y);
   allBirds.push_back(newBird);
   isFlying.push_back(false);
   actualTarget.push_back(Pos(x, y));
   currentPath.push_back(CurrentPathInfo(Pos(x, y), Pos(x, y), Pos(x, y)));

   glBindVertexArray(VAO);

   const unsigned int vertexOffset = 9 * boidsIn * sizeof(float); 
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferSubData(
         GL_ARRAY_BUFFER, vertexOffset, sizeof(newBird.pos), newBird.pos
   );

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
   
   boidsIn++;
}

Pos calculateCurvePoint(CurrentPathInfo cp)
{
   // float distX = abs(cp.start.x - cp.end.x) / 2.0;
   // return Pos(distX + cp.start.x, cp.end.y);
   std::uniform_int_distribution<> distrX(-680, 680);
   std::uniform_int_distribution<> distrY(-480, 480);

   return Pos(distrX(gen) / 680.0, distrY(gen) / 480.0);
}

void generateNewTarget(int bird)
{
   std::uniform_int_distribution<> distrX(-680, 680);
   std::uniform_int_distribution<> distrY(-480, 480);

   isFlying[bird] = true;
   actualTarget[bird].x = distrX(gen) / 680.0;
   actualTarget[bird].y = distrY(gen) / 480.0;
   currentPath[bird].start = allBirds[bird].center;
   currentPath[bird].end = actualTarget[bird];
   currentPath[bird].curve = calculateCurvePoint(currentPath[bird]);
   currentPath[bird].t = 0.0;
}

void updateVerticesBird(const int bird)
{
   Pos centerBird = allBirds[bird].center;
   allBirds[bird] = Bird(centerBird.x, centerBird.y);
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

Pos quadraticBezier(CurrentPathInfo cp)
{
   Pos newPos;

   newPos.x = std::pow(1 - cp.t, 2) * cp.start.x +
      (1 - cp.t) * 2 * cp.t * cp.curve.x + cp.t * cp.t * cp.end.x;
   newPos.y = std::pow(1 - cp.t, 2) * cp.start.y +
      (1 - cp.t) * 2 * cp.t * cp.curve.y + cp.t * cp.t * cp.end.y;

   return newPos;
}

void moveToTarget(int boid)
{
   
   // Method #1
   //const float speed = 0.001;
   //const float step = 0.001;

   //float directionX = (actualTarget[boid].x > allBirds[boid].center.x)? 1.0 : -1.0;
   //float directionY = (actualTarget[boid].y > allBirds[boid].center.y)? 1.0 : -1.0;

   //if (abs(allBirds[boid].center.x - actualTarget[boid].x) > 0.001)
   //   allBirds[boid].center.x += step * directionX * speed;
   //if (abs(allBirds[boid].center.y - actualTarget[boid].y) > 0.001)
   //   allBirds[boid].center.y += step * directionY * speed;

   // Method #2
   // ....

   // Method #3
   const float step = 0.05;
   Pos newPos = quadraticBezier(currentPath[boid]);
   currentPath[boid].t += step * 0.001;

   allBirds[boid].center = newPos;


   updateVerticesBird(boid);
}

void randomFly()
{
   for (int i = 0; i < boidsIn; i++) {

      if (isFlying[i] == false) {
         generateNewTarget(i);
         isFlying[i] = true;

      } else {
         moveToTarget(i);
         updateBuffer(i);
         
         // Method 1
         //if (abs(allBirds[i].center.x - actualTarget[i].x) < 0.001 &&
         //    abs(allBirds[i].center.y - actualTarget[i].y) < 0.001) {
         //      isFlying[i] = false;
         //}
         if (abs(currentPath[i].t - 1.0) < 0.0001)
            isFlying[i] = false;
      }
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
