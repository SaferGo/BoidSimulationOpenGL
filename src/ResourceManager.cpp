#include <boidSimulation/ResourceManager.h>

#include <exception>

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager()
{
   //
}

static ResourceManager* getInstance()
{
   if (instance == nullptr)
      instance = new ResourceManager();

   return instance;
}

void loadShader()
{
   //
}

