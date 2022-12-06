#include <simplege/simplege.h>

#include "compositor.h"

using json = nlohmann::json;

namespace SimpleGE
{
  Component::SetupResult CompositorComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult CompositorComponent::Setup(const Description& descr)
  {
    auto vsSource = Resources::Get<TextAsset>(descr.vertexShader);
    auto fsSource = Resources::Get<TextAsset>(descr.fragmentShader);

    Ensures(vsSource != nullptr);
    Ensures(fsSource != nullptr);

    auto vs = Graphic::CreateVertexShader(*vsSource);
    auto fs = Graphic::CreateFragmentShader(*fsSource);

    shader = Graphic::Program::Create(vs, fs);
    vao = std::make_shared<Graphic::GL::VertexArray>();

    return {this, {}};
  }

  void CompositorComponent::Bind()
  {
    vao->Bind();
    shader->Use();
  }

  void CompositorComponent::Unbind() { vao->Unbind(); }
} // namespace SimpleGE
