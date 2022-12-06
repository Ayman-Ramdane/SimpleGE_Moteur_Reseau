#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE::Graphic
{
  template<ShaderType Type>
  inline ShaderRef<Type> CreateShader(std::string_view source)
  {
    return std::make_shared<GL::Shader<Type>>(source);
  }

  using VertexShaderRef = ShaderRef<ShaderType::Vertex>;
  inline VertexShaderRef CreateVertexShader(std::string_view source)
  {
    return CreateShader<ShaderType::Vertex>(source);
  }

  using FragmentShaderRef = ShaderRef<ShaderType::Fragment>;
  inline FragmentShaderRef CreateFragmentShader(std::string_view source)
  {
    return CreateShader<ShaderType::Fragment>(source);
  }
} // namespace SimpleGE::Graphic