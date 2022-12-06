#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>

#include <simplege/graphics/buffer.h>
#include <simplege/graphics/shader.h>

#include <simplege/image.h>

namespace SimpleGE
{
  class Image;

  class TextureComponent : public virtual Component
  {
  public:
    struct Description
    {
      std::string texture;
      std::string vertexShader;
      std::string fragmentShader;
    };

    TextureComponent(Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void Bind(const Graphic::VertexBufferRawRef& vertexBuffer, const Graphic::IndexBufferRawRef& indexBuffer);
    void Unbind();

    [[nodiscard]] ImageRef GetTexture() const { return image; }

  private:
    ImageRef image;
    Graphic::Texture2DRef texture;
    Graphic::VertexShaderRef vertexShader;
    Graphic::FragmentShaderRef fragmentShader;
    Graphic::VertexArrayRef vao;
    Graphic::ProgramRef program;
    Graphic::ShaderAttributeRef vertexPositionAttrib;
    Graphic::ShaderAttributeRef textureCoordAttrib;
    Graphic::ShaderUniformRef pUniform;
    Graphic::ShaderUniformRef mvUniform;
    Graphic::ShaderUniformRef uSampler;
  };
} // namespace SimpleGE
