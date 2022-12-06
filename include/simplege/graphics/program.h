#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/shader.h>

namespace SimpleGE::Graphic
{
  class Program
  {
  public:
    template<ShaderType... Types>
    static ProgramRef Create(const ShaderRef<Types>&... shaders)
    {
      return std::make_shared<Program>(shaders...);
    }

    template<ShaderType... Types>
    Program(const ShaderRef<Types>&... shaders) : Program()
    {
      (Attach(shaders), ...);
      Link();
    }

    void Use() { program.Use(); }
    [[nodiscard]] ShaderAttributeRef GetAttribute(std::string_view name)
    {
      return std::make_shared<GL::ShaderAttribute>(program.GetAttribute(name));
    }

    [[nodiscard]] ShaderUniformRef GetUniform(std::string_view name)
    {
      return std::make_shared<GL::ShaderUniform>(program.GetUniform(name));
    }

  private:
    Program() = default;

    template<ShaderType Type>
    void Attach(const ShaderRef<Type>& shader)
    {
      Expects(shader != nullptr);
      program.Attach(*shader);
    }

    void Link() { program.Link(); }

    GL::Program program;
  };
} // namespace SimpleGE::Graphic