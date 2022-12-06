#pragma once

#include <gl-cpp/pch/precomp.h>

#include <gl-cpp/renderbuffer.h>
#include <gl-cpp/texture.h>

namespace SimpleGE::Graphic::GL
{
  enum class Attachment : GLenum
  {
    Depth = GL_DEPTH_ATTACHMENT,
    Color0 = GL_COLOR_ATTACHMENT0,
  };

  class Framebuffer
  {
  private:
    static constexpr GLuint Invalid = ~0;
    static constexpr GLuint DefaultFB = 0;

  public:
    static Framebuffer& GetDefault()
    {
      static Framebuffer defaultFB(DefaultFB);
      return defaultFB;
    }

    Framebuffer() { glCreateFramebuffers(1, &framebuffer); }

    Framebuffer(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& other) noexcept : framebuffer(other.framebuffer) { other.framebuffer = Invalid; }

    ~Framebuffer() { Close(); }

    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer& operator=(Framebuffer&& other) noexcept
    {
      Close();
      framebuffer = other.framebuffer;
      other.framebuffer = Invalid;
      return *this;
    }

    /*[[deprecated]]*/ void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); }

    /*[[deprecated]]*/ void Unbind() const // NOLINT(readability-convert-member-functions-to-static)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Attach(Attachment attachment, const Renderbuffer& renderbuffer) const
    {
      glNamedFramebufferRenderbuffer(framebuffer, (GLenum) attachment, GL_RENDERBUFFER, renderbuffer.GetNative());
    }

    template<TextureTarget TexTarget>
    void Attach(Attachment attachment, const Texture<TexTarget>& texture)
    {
      glNamedFramebufferTexture(framebuffer, (GLenum) attachment, texture.GetNative(), 0);
    }

    void ClearDepth(float depth = 1.F) const { glClearNamedFramebufferfv(framebuffer, GL_DEPTH, 0, &depth); }

    void ClearColor(const ColorFloat& color, GLint index = 0) const
    {
      glClearNamedFramebufferfv(framebuffer, GL_COLOR, index, std::bit_cast<float*>(&color));
    }

    void Validate() const
    {
      auto result = glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER);
      Expects(result == GL_FRAMEBUFFER_COMPLETE);
    }

    [[nodiscard]] GLuint GetNative() const
    {
      Expects(framebuffer != Invalid);
      return framebuffer;
    }

  private:
    explicit Framebuffer(GLuint value) : framebuffer(value) {}

    void Close()
    {
      if (framebuffer != Invalid && framebuffer != DefaultFB)
      {
        glDeleteFramebuffers(1, &framebuffer);
      }
      framebuffer = Invalid;
    }

    GLuint framebuffer{Invalid};
  };
} // namespace SimpleGE::Graphic::GL