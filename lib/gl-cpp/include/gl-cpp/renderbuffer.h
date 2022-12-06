#pragma once

#include <gl-cpp/pch/precomp.h>

namespace SimpleGE::Graphic::GL
{
  enum class RenderbufferStorage : GLenum
  {
    Depth = GL_DEPTH_COMPONENT16,
  };

  class Renderbuffer
  {
  private:
    static constexpr GLuint Invalid = ~0;

  public:
    Renderbuffer(RenderbufferStorage storage, GLsizei width, GLsizei height)
    {
      glCreateRenderbuffers(1, &renderbuffer);
      glNamedRenderbufferStorage(renderbuffer, (GLenum) storage, width, height);
    }

    Renderbuffer(const Renderbuffer&) = delete;

    Renderbuffer(Renderbuffer&& other) noexcept : renderbuffer(other.renderbuffer) { other.renderbuffer = Invalid; }

    ~Renderbuffer() { Close(); }

    Renderbuffer& operator=(const Renderbuffer&) = delete;

    Renderbuffer& operator=(Renderbuffer&& other) noexcept
    {
      Close();
      renderbuffer = other.renderbuffer;
      other.renderbuffer = Invalid;
      return *this;
    }

    /*[[deprecated]]*/ void Bind() const { glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer); }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    /*[[deprecated]]*/ void Unbind() const { glBindRenderbuffer(GL_RENDERBUFFER, 0); }

    [[nodiscard]] GLuint GetNative() const
    {
      Expects(renderbuffer != Invalid);
      return renderbuffer;
    }

  private:
    void Close()
    {
      if (renderbuffer != Invalid)
      {
        glDeleteRenderbuffers(1, &renderbuffer);
      }
      renderbuffer = Invalid;
    }

    GLuint renderbuffer{Invalid};
  };
} // namespace SimpleGE::Graphic::GL