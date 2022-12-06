#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class Entity;
  using EntityRef = std::shared_ptr<Entity>;

  class Image;
  using ImageRef = std::shared_ptr<Image>;

  class TextAsset;
  using TextAssetRef = std::shared_ptr<TextAsset>;

  namespace Graphic
  {
    using BufferType = GL::BufferType;
    using BufferUsage = GL::BufferUsage;

    using ShaderType = GL::ShaderType;

    template<BufferType Type, typename ElemType, BufferUsage Usage>
    using BufferRef = std::shared_ptr<GL::Buffer<Type, ElemType, Usage>>;

    using FramebufferRef = std::shared_ptr<GL::Framebuffer>;

    class Program;
    using ProgramRef = std::shared_ptr<Program>;

    class RenderTexture;
    using RenderTextureRef = std::shared_ptr<RenderTexture>;

    template<ShaderType Type>
    using ShaderRef = std::shared_ptr<GL::Shader<Type>>;

    using ShaderAttributeRef = std::shared_ptr<GL::ShaderAttribute>;
    using ShaderUniformRef = std::shared_ptr<GL::ShaderUniform>;

    using Texture2DRef = std::shared_ptr<GL::Texture2D>;

    using VertexArrayRef = std::shared_ptr<GL::VertexArray>;

  } // namespace Graphic

  namespace Network
  {
    class Connection;
    using ConnectionRef = std::shared_ptr<Connection>;

    class Server;
    using ServerRef = std::shared_ptr<Server>;
  } // namespace Network
} // namespace SimpleGE