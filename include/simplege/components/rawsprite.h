#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/texture.h>
#include <simplege/components/visual.h>

namespace SimpleGE
{
  class RawSpriteComponent : public VisualComponent, public TextureComponent
  {
  public:
    static constexpr auto Type = "RawSprite";

    struct Description : public TextureComponent::Description
    {
      float width{};
      float height{};
      float scale{1.F};
    };

    RawSpriteComponent(Entity& owner) : TextureComponent(owner), Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void Display(const Timing& timing) override;

  private:
    void UpdateComponents(const Description& descr, Image& texture);

    struct Vertex
    {
      float x, y, z;
      float u, v;
    };

    Graphic::VertexBufferRef<Vertex, Graphic::BufferUsage::Static> vertexBuffer;
    std::array<Vertex, 4> vertices{};
    static constexpr std::array<std::uint16_t, 6> Indices{0, 1, 2, 2, 3, 0};
    Graphic::IndexBufferRef<std::uint16_t, Graphic::BufferUsage::Static> indexBuffer;
  };
} // namespace SimpleGE
