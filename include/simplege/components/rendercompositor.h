#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/compositor.h>

#include <simplege/graphics/buffer.h>

namespace SimpleGE
{
  class RenderCompositorComponent : public CompositorComponent
  {
  public:
    static constexpr auto Type = "RenderCompositor";

    using Description = CompositorComponent::Description;

    RenderCompositorComponent(Entity& owner) : CompositorComponent(owner), Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    Graphic::RenderTextureRef Compose(const Graphic::RenderTextureRef& texture) override;

  private:
    struct RenderCompositorVertex
    {
      static constexpr auto ItemCount = 2;
      float x, y;
    };

    static constexpr std::array<RenderCompositorVertex, 4> Vertices{{
        {-1.F, -1.F},
        {1.F, -1.F},
        {1.F, 1.F},
        {-1.F, 1.F},
    }};

    Graphic::ShaderAttributeRef positionAttrib;
    Graphic::ShaderUniformRef uSampler;
    Graphic::VertexBufferRef<RenderCompositorVertex, Graphic::BufferUsage::Static> screenQuad;
    static constexpr std::array<std::uint16_t, 6> Indices{0, 1, 2, 2, 3, 0};
    Graphic::IndexBufferRef<std::uint16_t, Graphic::BufferUsage::Static> indexBuffer;
  };
} // namespace SimpleGE
