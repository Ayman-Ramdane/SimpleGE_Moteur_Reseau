#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/compositor.h>

#include <simplege/graphics/program.h>

namespace SimpleGE
{
  static void from_json(const nlohmann::json& j, CompositorComponent::Description& desc)
  {
    j.at("vertexShader").get_to(desc.vertexShader);
    j.at("fragmentShader").get_to(desc.fragmentShader);
  }
} // namespace SimpleGE
