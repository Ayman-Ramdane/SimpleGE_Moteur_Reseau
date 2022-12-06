#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/texture.h>

namespace SimpleGE::Graphic
{
  class RenderTexture
  {
  public:
    struct Description
    {
      std::uint32_t width;
      std::uint32_t height;
      GL::TextureFilter minFilter;
      GL::TextureFilter magFilter;
      GL::TextureWrap horizWrap;
      GL::TextureWrap vertWrap;
    };

    static RenderTextureRef Create(const Description& descr) { return std::make_shared<RenderTexture>(descr); }

    explicit RenderTexture(const Description& descr)
        : colorBuffer(GL::TextureInternalFormat::RGBA, {descr.width, descr.height}),
          depthBuffer(GL::RenderbufferStorage::Depth, descr.width, descr.height)
    {
      colorBuffer.SetFilter(descr.minFilter, descr.magFilter);
      colorBuffer.SetWrap(descr.horizWrap, descr.vertWrap);

      rttFrameBuffer.Attach(GL::Attachment::Color0, colorBuffer);
      rttFrameBuffer.Attach(GL::Attachment::Depth, depthBuffer);

      rttFrameBuffer.Validate();
    }

    void ClearDepth(float depth = 1.F) const { rttFrameBuffer.ClearDepth(depth); }

    void ClearColor(const GL::ColorFloat& color) const { rttFrameBuffer.ClearColor(color); }

    void BindFramebuffer()
    {
      rttFrameBuffer.Bind();
      depthBuffer.Bind();
    }

    void UnbindFramebuffer()
    {
      rttFrameBuffer.Unbind();
      depthBuffer.Unbind();
    }

    [[nodiscard]] GL::Texture2D& GetTexture() { return colorBuffer; }

  private:
    GL::Texture2D colorBuffer;
    GL::Framebuffer rttFrameBuffer;
    GL::Renderbuffer depthBuffer;
  };
} // namespace SimpleGE::Graphic