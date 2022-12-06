#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/texture.h>
#include <simplege/image.h>

namespace SimpleGE::Graphic
{
  struct TextureDescription
  {
    const ImageRef& image;
    GL::TextureFilter minFilter;
    GL::TextureFilter magFilter;
    GL::TextureWrap horizWrap;
    GL::TextureWrap vertWrap;
  };

  inline Texture2DRef CreateTexture(const TextureDescription& descr)
  {
    GL::Texture2D texture(GL::TextureInternalFormat::RGBA, {descr.image->Width(), descr.image->Height()});
    texture.Update(descr.image->Pixels());
    texture.SetFilter(descr.minFilter, descr.magFilter);
    texture.SetWrap(descr.horizWrap, descr.vertWrap);

    return std::make_shared<GL::Texture2D>(std::move(texture));
  }
} // namespace SimpleGE::Graphic