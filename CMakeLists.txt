# The name of this project is "GAM300_VULKAN". CMakeLists files in this project can
# refer to the root source directory of the project as ${GAM300_VULKAN_SOURCE_DIR} and
# to the root binary directory of the project as ${GAM300_VULKAN_BINARY_DIR}.
cmake_minimum_required(VERSION 3.10.2)
project (GAM300_VULKAN)

set(BUILD_STATIC_LIBS ON)

add_subdirectory(lib/SDL)
target_include_directories(SDL2 INTERFACE ${SDL2_INCLUDE_DIR})

# Find GLM
set(GLM_INCLUDE_DIR lib/glm)
add_library(GLM INTERFACE)
target_include_directories(GLM INTERFACE ${GLM_INCLUDE_DIR})

# Cross Graphics Dependencies
find_path(VULKAN_INCLUDE_DIR NAMES vulkan/vulkan.h HINTS
    "$ENV{VULKAN_SDK}/include"
    "$ENV{VULKAN_SDK}/Include"
    "$ENV{VK_SDK_PATH}/Include")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    find_library(VULKAN_LIBRARY
        NAMES vulkan-1 vulkan vulkan.1
        HINTS
        "$ENV{VULKAN_SDK}/lib"
        "$ENV{VULKAN_SDK}/Lib"
        "$ENV{VULKAN_SDK}/Bin"
        "$ENV{VK_SDK_PATH}/Bin")
else()
    find_library(VULKAN_LIBRARY
                NAMES vulkan-1 vulkan vulkan.1
                HINTS
        "$ENV{VULKAN_SDK}/Lib32"
        "$ENV{VULKAN_SDK}/Bin32"
        "$ENV{VK_SDK_PATH}/Bin32")
endif()

file(GLOB_RECURSE ALL_SRC src/**.cpp)
add_executable(${PROJECT_NAME} ${ALL_SRC})

target_link_libraries(
  ${PROJECT_NAME}
  ${VULKAN_LIBRARY}
  SDL2-static
  GLM
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${GLM_INCLUDE_DIR}
  PUBLIC ${SDL2_INCLUDE_DIR}
  PUBLIC ${VULKAN_INCLUDE_DIR}
)

if(WIN32)
  add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
endif()
