#pragma once

#include <gl-cpp/pch/precomp.h>

namespace SimpleGE::Graphic::GL
{
  template<typename T>
  struct Color
  {
    T red;
    T green;
    T blue;
    T alpha;
  };

  using ColorU8 = Color<std::uint8_t>;
  using ColorFloat = Color<float>;
} // namespace SimpleGE::Graphic::GL