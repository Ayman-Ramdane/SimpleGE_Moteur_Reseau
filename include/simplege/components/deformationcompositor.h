#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/compositor.h>

#include <simplege/graphics/buffer.h>

namespace SimpleGE
{
  class DeformationCompositorComponent : public CompositorComponent
  {
  public:
    static constexpr auto Type = "DeformationCompositor";

    struct Description : public CompositorComponent::Description
    {
      float speed;
      float scale;
      std::string source;
      std::string intensity;
    };

    DeformationCompositorComponent(Entity& owner) : CompositorComponent(owner), Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    Graphic::RenderTextureRef Compose(const Graphic::RenderTextureRef& texture) override;

  private:
    void OnEnabled() override;

    struct DeformationCompositorVertex
    {
      static constexpr auto ItemCount = 2;
      float x, y;
    };

    static constexpr std::array<DeformationCompositorVertex, 4> Vertices{{
        {-1.F, -1.F},
        {1.F, -1.F},
        {1.F, 1.F},
        {-1.F, 1.F},
    }};

    float speed{};
    float scale{};
    std::chrono::time_point<std::chrono::steady_clock> start;
    Graphic::Texture2DRef deformation;
    Graphic::Texture2DRef intensity;
    Graphic::ShaderAttributeRef positionAttrib;
    Graphic::ShaderUniformRef uSampler;
    Graphic::ShaderUniformRef uDeformation;
    Graphic::ShaderUniformRef uIntensity;
    Graphic::ShaderUniformRef uTime;
    Graphic::ShaderUniformRef uScale;
    Graphic::VertexBufferRef<DeformationCompositorVertex, Graphic::BufferUsage::Static> screenQuad;
    static constexpr std::array<std::uint16_t, 6> Indices{0, 1, 2, 2, 3, 0};
    Graphic::IndexBufferRef<std::uint16_t, Graphic::BufferUsage::Static> indexBuffer;
    Graphic::RenderTextureRef renderTexture;
  };
} // namespace SimpleGE
