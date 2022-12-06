#pragma once

#if !__has_include(<bit>)
#error Fonctionnalités C++ manquantes
#endif

#include <bit>
#include <functional>
#include <string_view>
#include <type_traits>

#include <gsl/gsl>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fmt/core.h>