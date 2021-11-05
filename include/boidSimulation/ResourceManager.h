#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <glShaderLoader.h>

class ResourceManager
{

public:
   static ResourceManager* getInstance();
   void loadShader();

private:
   static ResourceManager* instance;

   GLuint shaderProgram;

   ResourceManager();
   ResourceManager(const ResourceManager&) = delete;
   ResourceManager& operator=(const ResourceManager&) = delete;

};

#endif
