#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/graphics/rendertexture.h>

namespace SimpleGE
{
  class CompositorComponent : public virtual Component
  {
  public:
    struct Description
    {
      std::string vertexShader;
      std::string fragmentShader;
    };

    CompositorComponent(Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& desc);

    virtual Graphic::RenderTextureRef Compose(const Graphic::RenderTextureRef& texture) = 0;

  protected:
    void Bind();
    void Unbind();

    Graphic::ProgramRef shader;
    Graphic::VertexArrayRef vao;
  };
} // namespace SimpleGE
