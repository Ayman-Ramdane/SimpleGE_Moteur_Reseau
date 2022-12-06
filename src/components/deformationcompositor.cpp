#include <simplege/simplege.h>

#include "compositor.h"

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, DeformationCompositorComponent::Description& desc)
  {
    from_json(j, static_cast<CompositorComponent::Description&>(desc));

    j.at("speed").get_to(desc.speed);
    j.at("scale").get_to(desc.scale);
    j.at("source").get_to(desc.source);
    j.at("intensity").get_to(desc.intensity);
  }

  void DeformationCompositorComponent::OnEnabled() { start = std::chrono::steady_clock::now(); }

  Component::SetupResult DeformationCompositorComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult DeformationCompositorComponent::Setup(const Description& descr)
  {
    return {this,
            {{[]() { return CameraComponent::Current() != nullptr; },
              [this, descr]()
              {
                const auto* currentCamera = CameraComponent::Current();
                Ensures(currentCamera != nullptr);

                const auto Width = currentCamera->ViewWidth();
                const auto Height = currentCamera->ViewHeight();

                speed = descr.speed;
                scale = descr.scale;
                start = std::chrono::steady_clock::now();

                auto status = CompositorComponent::Setup(descr);
                Ensures(status.IsDone());
                Bind();

                auto deformationImage = Resources::Get<Image>(descr.source);
                auto intensityImage = Resources::Get<Image>(descr.intensity);

                Ensures(deformationImage != nullptr);
                Ensures(intensityImage != nullptr);

                {
                  using namespace Graphic;

                  deformation = CreateTexture({deformationImage, GL::TextureFilter::Linear, GL::TextureFilter::Linear,
                                               GL::TextureWrap::Wrap, GL::TextureWrap::Wrap});

                  intensity = CreateTexture({intensityImage, GL::TextureFilter::Linear, GL::TextureFilter::Linear,
                                             GL::TextureWrap::Clamp, GL::TextureWrap::Clamp});

                  renderTexture =
                      RenderTexture::Create({Width, Height, GL::TextureFilter::Nearest, GL::TextureFilter::Nearest,
                                             GL::TextureWrap::Clamp, GL::TextureWrap::Clamp});

                  screenQuad = CreateVertexBuffer<DeformationCompositorVertex, BufferUsage::Static>(Vertices.size());
                  screenQuad->Update(Vertices);

                  indexBuffer = CreateIndexBuffer<std::uint16_t, Graphic::BufferUsage::Static>(Indices.size());
                  indexBuffer->Update(Indices);
                }

                positionAttrib = shader->GetAttribute("aPosition");
                uSampler = shader->GetUniform("uSampler");
                uDeformation = shader->GetUniform("uDeformation");
                uIntensity = shader->GetUniform("uIntensity");
                uTime = shader->GetUniform("uTime");
                uScale = shader->GetUniform("uScale");

                renderTexture->ClearDepth();

                Unbind();
              }}}};
  }

  Graphic::RenderTextureRef DeformationCompositorComponent::Compose(const Graphic::RenderTextureRef& texture)
  {
    renderTexture->BindFramebuffer();

    renderTexture->ClearDepth();

    Bind();

    screenQuad->Bind();
    vao->Enable(*positionAttrib);
    constexpr auto PositionOffset = 0U;
    positionAttrib->Setup<float>(DeformationCompositorVertex::ItemCount, PositionOffset);

    indexBuffer->Bind();

    texture->GetTexture().BindUnit();
    uSampler->Set(texture->GetTexture());

    deformation->BindUnit();
    uDeformation->Set(*deformation);

    intensity->BindUnit();
    uIntensity->Set(*intensity);

    const auto elapsed = std::chrono::duration<float>(std::chrono::steady_clock::now() - start).count() * speed;
    uTime->Set(elapsed);

    uScale->Set(scale);

    Graphic::GL::DrawElements(Indices);

    vao->Disable(*positionAttrib);

    intensity->UnbindUnit();
    deformation->UnbindUnit();
    texture->GetTexture().UnbindUnit();

    Unbind();

    if (elapsed >= 1.F)
    {
      Disable();
    }

    return renderTexture;
  }
} // namespace SimpleGE