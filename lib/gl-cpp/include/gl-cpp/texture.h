#pragma once

#include <gl-cpp/pch/precomp.h>

#include <gl-cpp/basetypes.h>

namespace SimpleGE::Graphic::GL
{
  enum class TextureTarget : GLenum
  {
    Tex2D = GL_TEXTURE_2D,
  };

  enum class TextureInternalFormat : GLenum
  {
    RGBA = GL_RGBA8,
  };

  enum class TexturePixelFormat : GLenum
  {
    RGBA = GL_RGBA,
  };

  enum class TextureFilter : GLint
  {
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
  };

  enum class TextureWrap : GLint
  {
    Clamp = GL_CLAMP_TO_EDGE,
    Wrap = GL_REPEAT,
  };

  template<typename Type>
  struct TextureType
  {
  };

  template<>
  struct TextureType<std::uint8_t>
  {
    static constexpr auto Type = GL_UNSIGNED_BYTE;
  };

  template<typename FieldType, TexturePixelFormat Format>
  struct TexturePixel
  {
  };

  template<typename FieldType>
  struct TexturePixel<FieldType, TexturePixelFormat::RGBA>
  {
    static constexpr auto GLFieldType = TextureType<FieldType>::Type;
    static constexpr auto PixelFormat = TexturePixelFormat::RGBA;

    FieldType r;
    FieldType g;
    FieldType b;
    FieldType a;
  };

  template<TextureTarget TexTarget>
  struct TextureImage
  {
  };

  template<typename ColorType>
  struct TexturePixelMapping
  {
  };

  template<typename Type>
  struct TexturePixelMapping<Color<Type>>
  {
    using PixelMapping = TexturePixel<Type, TexturePixelFormat::RGBA>;
  };

  template<>
  struct TextureImage<TextureTarget::Tex2D>
  {
    using Dimensions = std::array<std::uint32_t, 2>;

    static void Alloc(GLuint texture, TextureInternalFormat internalFormat, const Dimensions& dim)
    {
      glTextureStorage2D(texture, 1, (GLint) internalFormat, dim[0], dim[1]);
    }

    template<typename ColorType>
    static void Update(GLuint texture, const Dimensions& dim, gsl::span<const ColorType> pixels)
    {
      using PixelMapping = typename TexturePixelMapping<ColorType>::PixelMapping;
      glTextureSubImage2D(texture, 0, 0, 0, dim[0], dim[1], (GLenum) PixelMapping::PixelFormat,
                          (GLenum) PixelMapping::GLFieldType, pixels.data());
    }
  };

  class TextureSlotAllocator
  {
  public:
    [[nodiscard]] static TextureSlotAllocator& Instance()
    {
      static TextureSlotAllocator instance;
      return instance;
    }

    static GLuint Alloc() { return Instance().AllocImpl(); }

    static void Free(GLuint slot) { Instance().FreeImpl(slot); }

  private:
    GLuint AllocImpl()
    {
      if (free.empty())
      {
        return next++;
      }
      auto slot = free.back();
      free.pop_back();
      return slot;
    }

    void FreeImpl(GLuint slot)
    {
      free.push_back(slot);
      std::sort(free.begin(), free.end(), std::greater<>());
    }

    std::vector<GLuint> free;
    GLuint next{};
  };

  class TextureBase
  {
  protected:
    static constexpr GLuint Invalid = ~0;

  public:
    TextureBase(const TextureBase&) = delete;

    ~TextureBase() { Close(); }

    TextureBase& operator=(const TextureBase&) = delete;

    void SetFilter(TextureFilter min, TextureFilter mag) const
    {
      glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, (GLint) min);
      glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, (GLint) mag);
    }

    void SetWrap(TextureWrap horiz, TextureWrap vert) const
    {
      glTextureParameteri(texture, GL_TEXTURE_WRAP_S, (GLint) horiz);
      glTextureParameteri(texture, GL_TEXTURE_WRAP_T, (GLint) vert);
    }

    void BindUnit()
    {
      Expects(boundUnit == Invalid);
      boundUnit = TextureSlotAllocator::Alloc();
      glBindTextureUnit(boundUnit, texture);
    }

    void UnbindUnit()
    {
      Expects(boundUnit != Invalid);
      glBindTextureUnit(boundUnit, 0);
      TextureSlotAllocator::Free(boundUnit);
      boundUnit = Invalid;
    }

    [[nodiscard]] GLuint GetBoundUnit() const { return boundUnit; }

    [[nodiscard]] GLuint GetNative() const
    {
      Expects(texture != Invalid);
      return texture;
    }

  protected:
    explicit TextureBase(TextureTarget texTarget) { glCreateTextures((GLenum) texTarget, 1, &texture); }

    TextureBase(TextureBase&& other) noexcept : texture(other.texture), boundUnit(other.boundUnit)
    {
      other.texture = Invalid;
    }

    TextureBase& operator=(TextureBase&& other) noexcept
    {
      Close();
      texture = other.texture;
      boundUnit = other.boundUnit;
      other.texture = Invalid;
      return *this;
    }

    template<TextureTarget TexTarget>
    void Alloc(TextureInternalFormat internalFormat, const typename TextureImage<TexTarget>::Dimensions& dim)
    {
      TextureImage<TexTarget>::Alloc(texture, internalFormat, dim);
    }

    template<TextureTarget TexTarget, typename ColorType>
    void UpdateData(const typename TextureImage<TexTarget>::Dimensions& dim, gsl::span<const ColorType> pixels)
    {
      TextureImage<TexTarget>::Update(texture, dim, pixels);
    }

    template<TextureTarget TexTarget>
    void BindImpl() const
    {
      glBindTexture((GLenum) TexTarget, texture);
    }

    template<TextureTarget TexTarget>
    static void UnbindImpl()
    {
      glBindTexture((GLenum) TexTarget, 0);
    }

    void Close()
    {
      if (texture != Invalid)
      {
        glDeleteTextures(1, &texture);
      }
      texture = Invalid;
    }

    GLuint texture{Invalid};   // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
    GLuint boundUnit{Invalid}; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
  };

  template<TextureTarget TexTarget>
  class Texture : public TextureBase
  {
  private:
    using SelfType = Texture<TexTarget>;
    using TexImage = TextureImage<TexTarget>;

  public:
    Texture(TextureInternalFormat internalFormat, const typename TexImage::Dimensions& dim)
        : TextureBase(TexTarget), dimensions(dim)
    {
      Alloc<TexTarget>(internalFormat, dim);
    }

    Texture(const SelfType&) = delete;

    Texture(SelfType&& other) noexcept = default;

    ~Texture() = default;

    Texture& operator=(const SelfType&) = delete;

    Texture& operator=(SelfType&& other) noexcept
    {
      TextureBase::operator=(std::move(other));
      return *this;
    }

    /*[[deprecated]]*/ void Bind() const { TextureBase::BindImpl<TexTarget>(); }

    /*[[deprecated]]*/ static void Unbind() { TextureBase::UnbindImpl<TexTarget>(); }

    template<typename ColorType>
    void Update(gsl::span<const ColorType> pixels)
    {
      UpdateData<TexTarget>(dimensions, pixels);
    }

  private:
    typename TexImage::Dimensions dimensions;
  };

  using Texture2D = Texture<TextureTarget::Tex2D>;
} // namespace SimpleGE::Graphic::GL