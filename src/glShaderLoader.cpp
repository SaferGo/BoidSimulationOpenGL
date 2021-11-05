#include <boidSimulation/glShaderLoader.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>


std::string glShaderLoader::readFile(const char* filePath)
{
   std::string shaderContent;
   std::ifstream fileStream(filePath, std::ios::in);

   if (!fileStream.is_open())
      return "";

   std::string line;
   while (!fileStream.eof()) {
      std::getline(fileStream, line);
      shaderContent.append(line + "\n");
   }

   fileStream.close();
   
   return shaderContent;
}

void glShaderLoader::printShaderCompilationErrors(const GLuint shader)
{
   char infoLog[512];
   glGetShaderInfoLog(shader, 512, NULL, infoLog);
   
   std::cerr << infoLog << std::endl;
}

void glShaderLoader::printShaderLinkingErrors(const GLuint program)
{
   char infoLog[512];
   glGetProgramInfoLog(program, 512, NULL, infoLog);

   std::cerr << infoLog << std::endl;
}


GLuint glShaderLoader::loadShader(
      const char* vertexPath,
      const char* fragmentPath
){
   GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
   GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

   const std::string vertexShaderStr = readFile(vertexPath);
   const std::string fragmentShaderStr = readFile(fragmentPath);
   const char* vertexShaderChar = vertexShaderStr.c_str();
   const char* fragmentShaderChar = fragmentShaderStr.c_str();

   if (vertexShaderStr == "" || fragmentShaderStr == "")
      throw std::ios_base::failure("Error while reading the shader files");
   
   std::cout << "##### Compiling vertex shader ####\n";

   // Compilation of shaders
   
   glShaderSource(vertexShader, 1, &vertexShaderChar, NULL);
   glCompileShader(vertexShader);

   GLint success;

   glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
   if (success == GL_FALSE) {
      printShaderCompilationErrors(vertexShader);
      glDeleteShader(vertexShader);
      
      throw std::runtime_error("Error/s found while compiling vertex shader.");
   }

   std::cout << "Success!\n";

   std:: cout << "#### Compiling fragment shader ####\n";
   
   glShaderSource(fragmentShader, 1, &fragmentShaderChar, NULL);
   glCompileShader(fragmentShader);

   glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
   if (success == GL_FALSE) {
      printShaderCompilationErrors(fragmentShader);
      glDeleteShader(vertexShader);

      throw std::runtime_error("Error/s found while compiling fragment shader.");
   }

   std::cout << "Success!\n";

   // Linking of shaders

   std::cout << "#### Linking shaders to the program ###\n";
   
   GLuint program = glCreateProgram();
   glAttachShader(program, vertexShader);
   glAttachShader(program, fragmentShader);
   glLinkProgram(program);

   glGetProgramiv(program, GL_LINK_STATUS, &success);
   if (success == GL_FALSE) {
      printShaderLinkingErrors(program);
      glDeleteProgram(program);

      throw std::runtime_error("Error while linking shaders.");
   }

   std::cout << "Success!\n";

   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

   return program;
}
