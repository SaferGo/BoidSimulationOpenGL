#ifndef GLSHADERLOADER_H
#define GLSHADERLOADER_H

#include <string>

#include <glad/glad.h>

namespace glShaderLoader
{
   std::string readFile(const char* filePath);
   void printShaderCompilationErrors(const GLuint shader);
   void printShaderLinkingErrors(const GLuint program);
   GLuint loadShader(const char* vertexPath, const char* fragmentPath);
}

#endif
