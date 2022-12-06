#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class Image
  {
  public:
    static ImageRef Create(const std::vector<std::byte>& content) { return std::make_shared<Image>(content); }

    explicit Image(const std::vector<std::byte>& content);

    [[nodiscard]] std::uint32_t Width() const { return width; }
    [[nodiscard]] std::uint32_t Height() const { return height; }
    [[nodiscard]] gsl::span<const Graphic::GL::ColorU8> Pixels() const { return m_Content; }
    operator gsl::span<const Graphic::GL::ColorU8>() const { return Pixels(); }

  private:
    std::uint32_t width{};
    std::uint32_t height{};
    std::vector<Graphic::GL::ColorU8> m_Content;
  };
} // namespace SimpleGE