#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE::Graphic
{
  template<BufferType Type, typename ElemType, BufferUsage Usage>
  inline BufferRef<Type, ElemType, Usage> CreateBuffer(std::size_t size)
  {
    return std::make_shared<GL::Buffer<Type, ElemType, Usage>>(size);
  }

  using VertexBufferRawRef = std::shared_ptr<GL::TypedBuffer<BufferType::Vertices>>;

  template<typename ElemType, BufferUsage Usage>
  using VertexBufferRef = BufferRef<BufferType::Vertices, ElemType, Usage>;
  template<typename ElemType, BufferUsage Usage>
  inline VertexBufferRef<ElemType, Usage> CreateVertexBuffer(std::size_t size)
  {
    return CreateBuffer<BufferType::Vertices, ElemType, Usage>(size);
  }

  using IndexBufferRawRef = std::shared_ptr<GL::TypedBuffer<BufferType::Indices>>;

  template<typename ElemType, BufferUsage Usage>
  using IndexBufferRef = BufferRef<BufferType::Indices, ElemType, Usage>;
  template<typename ElemType, BufferUsage Usage>
  inline IndexBufferRef<ElemType, Usage> CreateIndexBuffer(std::size_t size)
  {
    return CreateBuffer<BufferType::Indices, ElemType, Usage>(size);
  }
} // namespace SimpleGE::Graphic