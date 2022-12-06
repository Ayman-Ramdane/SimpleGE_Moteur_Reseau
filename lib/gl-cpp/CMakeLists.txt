cmake_minimum_required(VERSION 3.16)

project(GL_CPP VERSION 0.0.1 LANGUAGES CXX)

include(../../cmake/common.cmake)

FetchContent_Declare(glew
  GIT_REPOSITORY  https://github.com/Perlmint/glew-cmake.git
  GIT_TAG         glew-cmake-2.2.0
)
FetchContent_Declare(glfw
  GIT_REPOSITORY  https://github.com/glfw/glfw
  GIT_TAG         e7758c506dfcf258566f866c761c1b6bd7d98011
)

FetchContent_GetProperties(glew)
if(NOT glew_POPULATED)
  FetchContent_Populate(glew)
  set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/enable_cmp0048.cmake")
  add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
  unset(CMAKE_PROJECT_INCLUDE_BEFORE)
endif()

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(TARGET_NAME gl_cpp)

add_library(${TARGET_NAME} INTERFACE)

target_precompile_headers(${TARGET_NAME} INTERFACE include/gl-cpp/pch/precomp.h)
target_compile_features(${TARGET_NAME} INTERFACE cxx_std_20)

find_package(OpenGL REQUIRED)
add_dependencies(${TARGET_NAME} OpenGL::GL)

target_include_directories(${TARGET_NAME} SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${TARGET_NAME} SYSTEM INTERFACE ${glew_SOURCE_DIR}/include)
target_include_directories(${TARGET_NAME} SYSTEM INTERFACE ${glfw_SOURCE_DIR}/include)

target_link_libraries(${TARGET_NAME} INTERFACE OpenGL::GL)
target_link_libraries(${TARGET_NAME} INTERFACE libglew_static)
target_link_libraries(${TARGET_NAME} INTERFACE glfw)
