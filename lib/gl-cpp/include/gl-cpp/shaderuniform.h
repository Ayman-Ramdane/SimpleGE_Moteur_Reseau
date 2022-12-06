#pragma once

#include <gl-cpp/pch/precomp.h>

#include <gl-cpp/texture.h>

namespace SimpleGE::Graphic::GL
{
  template<typename Type>
  struct ShaderUniformTypeTraits
  {
  };

  template<>
  struct ShaderUniformTypeTraits<float>
  {
    static void Assign(GLint uniform, float val) { glUniform1f(uniform, val); }
  };

  template<TextureTarget TexTarget>
  struct ShaderUniformTypeTraits<Texture<TexTarget>>
  {
    static void Assign(GLint uniform, const Texture<TexTarget>& val) { glUniform1i(uniform, val.GetBoundUnit()); }
  };

  class ShaderUniform
  {
  private:
    static constexpr GLint Invalid = ~0;

  public:
    explicit ShaderUniform(GLint uniform) : uniform(uniform) {}

    template<typename Type>
    void Set(const Type& val)
    {
      ShaderUniformTypeTraits<Type>::Assign(uniform, val);
    }

    [[nodiscard]] GLint GetNative() const
    {
      Expects(uniform != Invalid);
      return uniform;
    }

  private:
    GLint uniform{Invalid};
  };
} // namespace SimpleGE::Graphic::GL