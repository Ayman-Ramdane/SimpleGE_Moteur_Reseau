#include <simplege/simplege.h>

#include "texture.h"

using json = nlohmann::json;

namespace SimpleGE
{
  namespace Graphic::GL
  {
    template<>
    struct ShaderUniformTypeTraits<Matrix<4, 4>>
    {
      static void Assign(GLint uniform, const Matrix<4, 4>& val)
      {
        glUniformMatrix4fv(uniform, 1, GL_FALSE, (const float*) val);
      }
    };
  } // namespace Graphic::GL

  Component::SetupResult TextureComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult TextureComponent::Setup(const Description& descr)
  {
    image = Resources::Get<Image>(descr.texture);
    Ensures(image != nullptr);

    {
      using namespace Graphic;
      texture = CreateTexture({image, GL::TextureFilter::Nearest, GL::TextureFilter::Nearest, GL::TextureWrap::Clamp,
                               GL::TextureWrap::Clamp});
    }

    auto vsSource = Resources::Get<TextAsset>(descr.vertexShader);
    Ensures(vsSource != nullptr);
    vertexShader = Graphic::CreateVertexShader(*vsSource);

    auto fsSource = Resources::Get<TextAsset>(descr.fragmentShader);
    Ensures(fsSource != nullptr);
    fragmentShader = Graphic::CreateFragmentShader(*fsSource);

    program = Graphic::Program::Create(vertexShader, fragmentShader);
    vao = std::make_shared<Graphic::GL::VertexArray>();

    vertexPositionAttrib = program->GetAttribute("aVertexPosition");
    textureCoordAttrib = program->GetAttribute("aTextureCoord");

    pUniform = program->GetUniform("uPMatrix");
    mvUniform = program->GetUniform("uMVMatrix");
    uSampler = program->GetUniform("uSampler");

    return {this, {}};
  }

  void TextureComponent::Bind(const Graphic::VertexBufferRawRef& vertexBuffer,
                              const Graphic::IndexBufferRawRef& indexBuffer)
  {
    // ## Constante *vertexSize*
    // Cette constante représente le nombre d'éléments d'un vertex,
    // soit 3 valeurs pour la position, et 2 pour la texture
    constexpr auto VertexSize = 3 + 2; // position(3d), texture(2d)
    constexpr auto Stride = VertexSize * sizeof(float);

    vao->Bind();
    program->Use();

    vertexBuffer->Bind();

    vao->Enable(*vertexPositionAttrib);
    vao->Enable(*textureCoordAttrib);
    constexpr auto PositionOffset = 0U;
    constexpr auto TexCoordOffset = 3U * sizeof(float);
    vertexPositionAttrib->Setup<float>(3, PositionOffset, Stride);
    textureCoordAttrib->Setup<float>(2, TexCoordOffset, Stride);

    indexBuffer->Bind();

    Expects(CameraComponent::Current() != nullptr);
    pUniform->Set(CameraComponent::Current()->Projection());
    Matrix<4, 4> identity{};
    mvUniform->Set(identity.LoadIdentity());

    texture->BindUnit();
    uSampler->Set(*texture);

    Graphic::GL::Enable(Graphic::GL::Capabilities::Blend);

    Graphic::GL::BlendFunc(Graphic::GL::BlendFuncT::SrcAlpha, Graphic::GL::BlendFuncT::OneMinusSrcAlpha);
  }

  void TextureComponent::Unbind()
  {
    texture->UnbindUnit();

    vao->Disable(*vertexPositionAttrib);
    vao->Disable(*textureCoordAttrib);
    vao->Unbind();
  }
} // namespace SimpleGE