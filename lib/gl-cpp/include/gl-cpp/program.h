#pragma once

#include <gl-cpp/pch/precomp.h>

#include <gl-cpp/shader.h>
#include <gl-cpp/shaderattribute.h>
#include <gl-cpp/shaderuniform.h>

namespace SimpleGE::Graphic::GL
{
  class Program
  {
  private:
    static constexpr GLuint Invalid = ~0;

  public:
    Program() : program(glCreateProgram()) { Expects(program > 0); }

    Program(const Program&) = delete;

    Program(Program&& other) noexcept : program(other.program) { other.program = Invalid; }

    ~Program() { Close(); }

    Program& operator=(const Program&) = delete;

    Program& operator=(Program&& other) noexcept
    {
      Close();
      program = other.program;
      other.program = Invalid;
      return *this;
    }

    void Use() const { glUseProgram(program); }

    template<ShaderType Type>
    void Attach(const Shader<Type>& shader)
    {
      glAttachShader(program, shader.GetNative());
    }

    void Link() const
    {
      glLinkProgram(program);

      GLint status = 0;
      glGetProgramiv(program, GL_LINK_STATUS, &status);
      if (status != GL_TRUE)
      {
        constexpr auto ShaderMaxLogSize = 1024;
        std::array<char, ShaderMaxLogSize> buffer{};
        GLsizei len = 0;

        glGetProgramInfoLog(program, buffer.max_size(), &len, buffer.data());
        fmt::print(stderr, "Initialisation du shader échouée: {}\n", std::string_view(buffer.data(), len));
        std::terminate();
      }
    }

    [[nodiscard]] ShaderAttribute GetAttribute(std::string_view name) const
    {
      return ShaderAttribute(glGetAttribLocation(program, name.data()));
    }

    [[nodiscard]] ShaderUniform GetUniform(std::string_view name) const
    {
      auto uniformLocation = glGetUniformLocation(program, name.data());
      if (uniformLocation < 0)
      {
        fmt::print(stderr, "Variable uniform {} inutilisée.\n", name);
      }
      return ShaderUniform(uniformLocation);
    }

  private:
    void Close()
    {
      if (program != Invalid)
      {
        glDeleteProgram(program);
      }
      program = Invalid;
    }

    GLuint program{Invalid};
  };
} // namespace SimpleGE::Graphic::GL