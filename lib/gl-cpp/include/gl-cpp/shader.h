#pragma once

#include <gl-cpp/pch/precomp.h>

namespace SimpleGE::Graphic::GL
{
  enum class ShaderType : GLenum
  {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
  };

  class ShaderBase
  {
  private:
    static constexpr GLuint Invalid = ~0;

  public:
    ShaderBase(const ShaderBase&) = delete;

    ~ShaderBase() { Close(); }

    ShaderBase& operator=(const ShaderBase&) = delete;

    [[nodiscard]] GLuint GetNative() const
    {
      Expects(shader != Invalid);
      return shader;
    }

  protected:
    ShaderBase(ShaderType type, std::string_view source) : shader(glCreateShader((GLenum) type))
    {
      const char* sourceStr = source.data();
      glShaderSource(shader, 1, &sourceStr, nullptr);
      glCompileShader(shader);

      GLint status = 0;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
      if (status != GL_TRUE)
      {
        constexpr auto ShaderMaxLogSize = 1024;
        std::array<char, ShaderMaxLogSize> buffer{};
        GLsizei len = 0;

        glGetShaderInfoLog(shader, buffer.max_size(), &len, buffer.data());
        fmt::print(stderr, "Erreur en compilant le shader: {}\n", std::string_view(buffer.data(), len));
        std::terminate();
      }
    }

    ShaderBase(ShaderBase&& other) noexcept : shader(other.shader) { other.shader = Invalid; }

    ShaderBase& operator=(ShaderBase&& other) noexcept
    {
      Close();
      shader = other.shader;
      other.shader = Invalid;
      return *this;
    }

  private:
    void Close()
    {
      if (shader != Invalid)
      {
        glDeleteShader(shader);
      }
      shader = Invalid;
    }

    GLuint shader{Invalid};
  };

  template<ShaderType Type>
  class Shader : public ShaderBase
  {
  private:
    using SelfType = Shader<Type>;

  public:
    explicit Shader(std::string_view source) : ShaderBase(Type, source) {}

    Shader(const SelfType&) = delete;

    Shader(SelfType&& other) noexcept = default;

    ~Shader() = default;

    Shader& operator=(const SelfType&) = delete;

    Shader& operator=(SelfType&& other) noexcept
    {
      ShaderBase::operator=(std::move(other));
      return *this;
    }
  };
} // namespace SimpleGE::Graphic::GL