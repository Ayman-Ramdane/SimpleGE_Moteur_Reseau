#pragma once

#include <gl-cpp/pch/precomp.h>

namespace SimpleGE::Graphic::GL
{
  template<typename Type>
  struct ShaderAttributeTypeTraits
  {
  };

  template<>
  struct ShaderAttributeTypeTraits<float>
  {
    static constexpr auto Type = GL_FLOAT;
  };

  class ShaderAttribute
  {
  private:
    static constexpr GLint Invalid = ~0;

  public:
    explicit ShaderAttribute(GLint attrib) : attrib(attrib) {}

    template<typename Type>
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void Setup(std::size_t nbElements, std::ptrdiff_t offset, std::size_t stride = 0, bool normalized = false)
    {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
      auto* attrOffset = std::bit_cast<GLvoid*>(offset);
      glVertexAttribPointer(attrib, nbElements, ShaderAttributeTypeTraits<Type>::Type, normalized ? GL_TRUE : GL_FALSE,
                            stride, attrOffset);
    }

    [[nodiscard]] GLint GetNative() const
    {
      Expects(attrib != Invalid);
      return attrib;
    }

  private:
    GLint attrib{Invalid};
  };
} // namespace SimpleGE::Graphic::GL