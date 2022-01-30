#ifndef GLSHADERLOADER_H
#define GLSHADERLOADER_H

#include <string>

#include <glad/glad.h>

namespace glShaderLoader
{
   std::string readFile(const char* filePath);
   void printShaderCompilationErrors(const unsigned int shader);
   void printShaderLinkingErrors(const unsigned int program);
   unsigned int loadShader(const char* vertexPath, const char* fragmentPath);
}

#endif
