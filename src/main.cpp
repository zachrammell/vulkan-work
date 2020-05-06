#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <cstdlib>

char const* appName = "vulkan300";

struct checkPhysicalDeviceProperties_return
{
  int usable = false;
  uint32_t queueFamilyIndex = std::numeric_limits<uint32_t>::max();
};

auto checkPhysicalDeviceProperties(vk::PhysicalDevice physicalDevice)
{
  auto constexpr failure = checkPhysicalDeviceProperties_return{};
  auto const deviceProperties = physicalDevice.getProperties();
  auto const deviceFeatures   = physicalDevice.getFeatures();

  uint32_t major_version = VK_VERSION_MAJOR(deviceProperties.apiVersion);
  uint32_t minor_version = VK_VERSION_MINOR(deviceProperties.apiVersion);
  uint32_t patch_version = VK_VERSION_PATCH(deviceProperties.apiVersion);

  if ((major_version < 1)
  || (deviceProperties.limits.maxImageDimension2D < 4096))
  {
    std::cerr << "Physical device " << physicalDevice << " doesn't support necessary features." << std::endl;
    return failure;
  }

  auto const queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
  for (uint32_t i = 0; i < queueFamilyProperties.size(); ++i)
  {
    auto& queueFamilyProperty = queueFamilyProperties[i];
    if ((queueFamilyProperty.queueCount > 0)
    && (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics))
    {
      return checkPhysicalDeviceProperties_return{ true, i };
    }
  }

  std::cerr << "Could not find queue family with required properties on physical device " << physicalDevice << std::endl;
  return failure;
}

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
    appName, VK_MAKE_VERSION(0, 0, 1),
    appName, VK_MAKE_VERSION(0, 0, 1),
    VK_API_VERSION_1_0
  };

  // initialize the VkInstanceCreateInfo structure
  vk::InstanceCreateInfo inst_info = {
    vk::InstanceCreateFlags{}, &app_info,
    0, nullptr,
    extensionCount, extensionNames.data()
  };

  vk::Instance instance;
  try
  {
    instance = vk::createInstance(inst_info);
  }
  catch (vk::SystemError& err)
  {
    std::cerr << "Vulkan SystemError: " << err.what() << std::endl;
    __debugbreak();
    exit(-1);
  }

  /* Logical Device Creation Section */
  vk::Device gpu;
  uint32_t selectedQueueFamilyIndex = std::numeric_limits<uint32_t>::max();
  try
  {
    vk::PhysicalDevice selectedPhysicalDevice;

    auto physicalDevices = instance.enumeratePhysicalDevices();
    /* Find the best physical device to use */
    for (vk::PhysicalDevice physicalDevice : physicalDevices)
    {
      auto const ret = checkPhysicalDeviceProperties(physicalDevice);
      if (ret.usable)
      {
        selectedPhysicalDevice = physicalDevice;
        selectedQueueFamilyIndex = ret.queueFamilyIndex;
        break;
      }
    }

    if (selectedPhysicalDevice == decltype(selectedPhysicalDevice){})
    {
      std::cerr << "No physical devices satisfying requirements are available." << std::endl;
      __debugbreak();
      exit(-1);
    }

    std::array<float, 1> queuePriorities = { 1.0f };

    vk::DeviceQueueCreateInfo queueCreateInfo = {
      {},
      selectedQueueFamilyIndex,
      queuePriorities.size(),
      queuePriorities.data()
    };

    vk::DeviceCreateInfo deviceCreateInfo = {
      {},
      1,
      &queueCreateInfo,
      0, nullptr,
      0,
      nullptr,
      nullptr
    };

    if (selectedPhysicalDevice.createDevice(&deviceCreateInfo, nullptr, &gpu) != vk::Result::eSuccess)
    {
      std::cerr << "Could not create Vulkan device!" << std::endl;
      __debugbreak();
      exit(-1);
    }
  }
  catch (vk::SystemError& err)
  {
    std::cerr << "Vulkan SystemError: " << err.what() << std::endl;
    __debugbreak();
    exit(-1);
  }

  vk::Queue graphicsQueue = gpu.getQueue(selectedQueueFamilyIndex, 0);

  vk::SurfaceKHR surface;
  if (!SDL_Vulkan_CreateSurface(window, instance, reinterpret_cast<VkSurfaceKHR*>(&surface)))
  {
    // failed to create a surface!
    std::cerr << "Failed to create Vulkan Surface with SDL." << std::endl;
    __debugbreak();
    exit(-1);
  }

  /* cleanup */
  SDL_DestroyWindow(window);
  gpu.destroy();
  instance.destroy();

  return 0;
}
