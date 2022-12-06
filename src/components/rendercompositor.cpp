#include <simplege/simplege.h>

#include "compositor.h"

using json = nlohmann::json;

namespace SimpleGE
{
  Component::SetupResult RenderCompositorComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult RenderCompositorComponent::Setup(const Description& descr)
  {
    auto status = CompositorComponent::Setup(descr);
    Ensures(status.IsDone());
    Bind();

    positionAttrib = shader->GetAttribute("aPosition");
    uSampler = shader->GetUniform("uSampler");

    screenQuad = Graphic::CreateVertexBuffer<RenderCompositorVertex, Graphic::BufferUsage::Static>(Vertices.size());
    screenQuad->Update(Vertices);

    indexBuffer = Graphic::CreateIndexBuffer<std::uint16_t, Graphic::BufferUsage::Static>(Indices.size());
    indexBuffer->Update(Indices);

    Unbind();

    return {this, {}};
  }

  Graphic::RenderTextureRef RenderCompositorComponent::Compose(const Graphic::RenderTextureRef& texture)
  {
    auto framebuffer = CameraComponent::Current()->GetFramebuffer();
    framebuffer->Bind();

    framebuffer->ClearDepth();

    Bind();

    screenQuad->Bind();
    vao->Enable(*positionAttrib);
    constexpr auto PositionOffset = 0U;
    positionAttrib->Setup<float>(RenderCompositorVertex::ItemCount, PositionOffset);

    indexBuffer->Bind();

    texture->GetTexture().BindUnit();
    uSampler->Set(texture->GetTexture());

    Graphic::GL::DrawElements(Indices);

    texture->GetTexture().UnbindUnit();

    vao->Disable(*positionAttrib);

    Unbind();

    // On ne s'en sert plus après ça
    return texture;
  }
} // namespace SimpleGE