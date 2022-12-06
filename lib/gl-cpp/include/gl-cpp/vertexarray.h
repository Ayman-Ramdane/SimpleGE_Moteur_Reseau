#pragma once

#include <gl-cpp/pch/precomp.h>

#include <gl-cpp/shaderattribute.h>

namespace SimpleGE::Graphic::GL
{
  class VertexArray
  {
  private:
    static constexpr GLuint Invalid = ~0;

  public:
    VertexArray() { glGenVertexArrays(1, &vao); }

    VertexArray(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept : vao(other.vao) { other.vao = Invalid; }

    ~VertexArray() { Close(); }

    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray& operator=(VertexArray&& other) noexcept
    {
      Close();
      vao = other.vao;
      other.vao = Invalid;
      return *this;
    }

    /*[[deprecated]]*/ void Bind() const { glBindVertexArray(vao); }

    /*[[deprecated]]*/ void Unbind() const // NOLINT(readability-convert-member-functions-to-static)
    {
      glBindVertexArray(0);
    }

    void Enable(const ShaderAttribute& attrib) const // NOLINT(readability-convert-member-functions-to-static)
    {
      glEnableVertexArrayAttrib(vao, attrib.GetNative());
    }

    void Disable(const ShaderAttribute& attrib) const // NOLINT(readability-convert-member-functions-to-static)
    {
      glDisableVertexArrayAttrib(vao, attrib.GetNative());
    }

  private:
    void Close()
    {
      if (vao != Invalid)
      {
        glDeleteVertexArrays(1, &vao);
      }
      vao = Invalid;
    }

    GLuint vao{Invalid};
  };
} // namespace SimpleGE::Graphic::GL