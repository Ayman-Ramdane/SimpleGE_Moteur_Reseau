cmake_minimum_required(VERSION 3.16)

project(ChickenDodge VERSION 0.0.1 LANGUAGES CXX)

set(TARGET_NAME chickendodge)

file(GLOB_RECURSE chickendodge_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(${TARGET_NAME} ${chickendodge_SOURCES})
target_precompile_headers(${TARGET_NAME} PRIVATE include/chickendodge/pch/precomp.h)
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)
target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_dependencies(${TARGET_NAME} simplege)
target_link_libraries(${TARGET_NAME} PRIVATE simplege)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E create_symlink
${CMAKE_SOURCE_DIR}/examples/chickendodge/data $<TARGET_FILE_DIR:chickendodge>/data)

set(TARGET_NAME chickendodge_server)

file(GLOB_RECURSE chickendodge_server_SOURCES CONFIGURE_DEPENDS server/*.cpp src/components/*.cpp)
add_executable(${TARGET_NAME} ${chickendodge_server_SOURCES})
target_precompile_headers(${TARGET_NAME} PRIVATE include/chickendodge/pch/precomp.h)
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)
target_include_directories(${TARGET_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_dependencies(${TARGET_NAME} simplege)
target_link_libraries(${TARGET_NAME} simplege)
