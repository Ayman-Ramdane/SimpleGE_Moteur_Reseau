#pragma once

#include <gl-cpp/pch/precomp.h>

namespace SimpleGE::Graphic::GL
{
  enum class BufferType : GLenum
  {
    Vertices = GL_ARRAY_BUFFER,
    Indices = GL_ELEMENT_ARRAY_BUFFER,
  };

  enum class BufferUsage : GLenum
  {
    Static = GL_STATIC_DRAW,
    Dynamic = GL_DYNAMIC_DRAW,
  };

  template<typename Type>
  struct BufferElemType
  {
  };

  template<>
  struct BufferElemType<std::uint16_t>
  {
    static constexpr auto Type = GL_UNSIGNED_SHORT;
  };

  class BufferBase
  {
  private:
    static constexpr GLuint Invalid = ~0;

  public:
    BufferBase(const BufferBase&) = delete;

    ~BufferBase() { Close(); }

    BufferBase& operator=(const BufferBase&) = delete;

    [[nodiscard]] GLsizeiptr GetSizeInBytes() const { return size; }

    [[nodiscard]] GLuint GetNative() const
    {
      Expects(buffer != Invalid);
      return buffer;
    }

  protected:
    BufferBase() { glCreateBuffers(1, &buffer); }

    BufferBase(BufferBase&& other) noexcept : buffer(other.buffer), size(other.size)
    {
      other.buffer = Invalid;
      other.size = 0;
    }

    BufferBase& operator=(BufferBase&& other) noexcept
    {
      Close();
      buffer = other.buffer;
      size = other.size;
      other.buffer = Invalid;
      other.size = 0;
      return *this;
    }

    template<BufferUsage Usage>
    void Init(GLsizeiptr bufSize)
    {
      glNamedBufferData(buffer, bufSize, nullptr, (GLenum) Usage);
      size = bufSize;
    }

    void UpdateBytes(gsl::span<const char> data, std::ptrdiff_t offset) const
    {
      glNamedBufferSubData(buffer, offset, static_cast<GLsizeiptr>(data.size_bytes()), data.data());
    }

    template<BufferType BufType>
    void BindImpl()
    {
      glBindBuffer((GLenum) BufType, buffer);
    }

    template<BufferType BufType>
    void UnbindImpl()
    {
      glBindBuffer((GLenum) BufType, 0);
    }

  private:
    void Close()
    {
      if (buffer != Invalid)
      {
        glDeleteBuffers(1, &buffer);
      }
      buffer = Invalid;
    }

    GLuint buffer{Invalid};
    GLsizeiptr size{};
  };

  template<BufferType BufType>
  class TypedBuffer : public BufferBase
  {
  public:
    /*[[deprecated]]*/ void Bind() { BindImpl<BufType>(); }
    /*[[deprecated]]*/ void Unbind() { UnbindImpl<BufType>(); }
  };

  template<BufferType BufType, typename ElemType, BufferUsage Usage>
  class Buffer : public TypedBuffer<BufType>
  {
  private:
    using SelfType = Buffer<BufType, ElemType, Usage>;

  public:
    explicit Buffer(std::size_t size) { BufferBase::Init<Usage>(size * sizeof(ElemType)); }

    Buffer(const SelfType&) = delete;

    Buffer(SelfType&& other) noexcept = default;

    ~Buffer() = default;

    Buffer& operator=(const SelfType&) = delete;

    Buffer& operator=(SelfType&& other) noexcept
    {
      BufferBase::operator=(std::move(other));
      return *this;
    }

    void Update(gsl::span<const ElemType> data, std::ptrdiff_t offset = 0)
    {
      BufferBase::UpdateBytes({std::bit_cast<const char*>(data.data()), data.size_bytes()}, offset * sizeof(ElemType));
    }

    [[nodiscard]] std::size_t GetSize() const { return BufferBase::GetSizeInBytes() / sizeof(ElemType); }
  };
} // namespace SimpleGE::Graphic::GL