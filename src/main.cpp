#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <cstdlib>

char const* appName = "vulkan300";

int main()
{
  /* VULKAN_KEY_START */
  SDL_Init(SDL_INIT_EVERYTHING);

  auto window = SDL_CreateWindow(appName,
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 1280, 720,
                                 SDL_WINDOW_VULKAN);

  uint32_t extensionCount;
  SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
  std::vector<const char*> extensionNames(extensionCount);
  SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames.data());

    // initialize the VkApplicationInfo structure
  vk::ApplicationInfo app_info = {
    appName, 1, appName, 1, VK_API_VERSION_1_0
  };

  // initialize the VkInstanceCreateInfo structure
  vk::InstanceCreateInfo inst_info = {
    vk::InstanceCreateFlags{}, &app_info,
    0, nullptr,
    extensionCount, extensionNames.data()
  };

  vk::Instance inst;
  try
  {
    inst = vk::createInstance(inst_info);
  }
  catch (std::runtime_error& e)
  {
    std::cerr << "Failed to create Vulkan instance: " << e.what() << std::endl;
  }

  vk::SurfaceKHR surface;
  if (!SDL_Vulkan_CreateSurface(window, inst, reinterpret_cast<VkSurfaceKHR*>(&surface)))
  {
    // failed to create a surface!
    __debugbreak();
  }


  /* cleanup */
  SDL_DestroyWindow(window);
  inst.destroy();

  return 0;
}
