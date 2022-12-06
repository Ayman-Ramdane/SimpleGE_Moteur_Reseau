#pragma once

#include <gl-cpp/pch/precomp.h>

#include <gl-cpp/basetypes.h>

namespace SimpleGE::Graphic::GL
{
  enum class Capabilities : GLenum
  {
    Blend = GL_BLEND,
    DebugOutput = GL_DEBUG_OUTPUT,
    DebugOutputSynchronous = GL_DEBUG_OUTPUT_SYNCHRONOUS,
    DepthTest = GL_DEPTH_TEST,
  };

  enum class DepthFuncT : GLenum
  {
    LEqual = GL_LEQUAL,
  };

  enum class BlendFuncT : GLenum
  {
    SrcAlpha = GL_SRC_ALPHA,
    OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
  };

  template<typename Type>
  struct DrawType
  {
  };

  template<>
  struct DrawType<std::uint16_t>
  {
    static constexpr auto Type = GL_UNSIGNED_SHORT;
  };

  struct DebugDrawCallHelper
  {
    static DebugDrawCallHelper& Instance()
    {
      static DebugDrawCallHelper instance;
      return instance;
    }

    std::function<void()> onDraw;
  };

  static inline void Enable(Capabilities cap) { glEnable((GLenum) cap); }

  static inline void Disable(Capabilities cap) { glDisable((GLenum) cap); }

  static inline void BlendFunc(BlendFuncT srcFactor, BlendFuncT destFactor)
  {
    glBlendFunc((GLenum) srcFactor, (GLenum) destFactor);
  }
  static inline void DepthFunc(DepthFuncT depthFunc) { glDepthFunc((GLenum) depthFunc); }

  // NOLINTNEXTLINE(readability-identifier-length)
  static inline void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) { glViewport(x, y, width, height); }

  template<typename Type>
  static inline void DrawElements(gsl::span<const Type> indices)
  {
    DebugDrawCallHelper::Instance().onDraw();
    glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), (GLenum) DrawType<Type>::Type, nullptr);
  }

  template<typename Type, size_t N>
  static inline void DrawElements(const std::array<Type, N>& indices)
  {
    DrawElements(gsl::span<const Type>(indices.data(), indices.size()));
  }
} // namespace SimpleGE::Graphic::GL