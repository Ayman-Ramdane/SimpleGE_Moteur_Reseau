cmake_minimum_required(VERSION 3.16)

project(SimpleGE VERSION 0.0.1 LANGUAGES CXX)

include(cmake/common.cmake)

add_subdirectory(lib/gl-cpp)

FetchContent_Declare(json
  GIT_REPOSITORY  https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG         v3.10.0
)
FetchContent_Declare(libpng
  GIT_REPOSITORY  https://github.com/glennrp/libpng.git
  GIT_TAG         v1.6.37
)
FetchContent_Declare(zlib
  GIT_REPOSITORY  https://github.com/madler/zlib.git
  GIT_TAG         v1.2.11
)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

SET(SKIP_INSTALL_EXPORT TRUE)

FetchContent_GetProperties(zlib)
if(NOT zlib_POPULATED)
    FetchContent_Populate(zlib)

    set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
    set(ZLIB_LIBRARY zlibstatic)

    execute_process(
      COMMAND           ${GIT_EXECUTABLE} apply ${CMAKE_CURRENT_LIST_DIR}/cmake/zlib.patch
      WORKING_DIRECTORY ${zlib_SOURCE_DIR}
      OUTPUT_QUIET
      ERROR_QUIET
    )

    set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/enable_cmp0048.cmake")
    add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} EXCLUDE_FROM_ALL)
    unset(CMAKE_PROJECT_INCLUDE_BEFORE)
endif()

FetchContent_GetProperties(libpng)
if(NOT libpng_POPULATED)
    FetchContent_Populate(libpng)

    set(PNG_SHARED OFF)

    set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/enable_cmp0077.cmake")
    add_subdirectory(${libpng_SOURCE_DIR} ${libpng_BINARY_DIR} EXCLUDE_FROM_ALL)
    unset(CMAKE_PROJECT_INCLUDE_BEFORE)
endif()

set(TARGET_NAME simplege)

file(GLOB_RECURSE simplege_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(${TARGET_NAME} STATIC ${simplege_SOURCES})

target_precompile_headers(${TARGET_NAME} PRIVATE include/simplege/pch/precomp.h)
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)

target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${libpng_SOURCE_DIR}/ ${libpng_BINARY_DIR}/)
target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${zlib_SOURCE_DIR}/)

find_package(Threads REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE Threads::Threads)
target_link_libraries(${TARGET_NAME} PUBLIC fmt::fmt)
target_link_libraries(${TARGET_NAME} PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(${TARGET_NAME} PRIVATE png_static)
target_link_libraries(${TARGET_NAME} PUBLIC gl_cpp)

if (WIN32)
  target_link_libraries(${TARGET_NAME} PRIVATE wsock32 ws2_32)
endif()

add_subdirectory(examples)
