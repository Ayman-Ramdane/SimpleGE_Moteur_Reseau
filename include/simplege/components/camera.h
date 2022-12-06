#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/math.h>
#include <simplege/timing.h>

struct GLFWwindow;

namespace SimpleGE
{
  using GraphicAPIWindowType = GLFWwindow;

  class CompositorComponent;

  class CameraComponent : public Component
  {
  public:
    static constexpr auto Type = "Camera";

    struct Description
    {
      std::string title;
      std::uint32_t viewHeight;
      std::uint32_t viewWidth;
      Graphic::GL::ColorFloat color;
      float height;
      float nearClip;
      float farClip;
      std::vector<ComponentReference<CompositorComponent>> compositors;
    };

    static CameraComponent* Current() { return current; }

    CameraComponent(Entity& owner);
    ~CameraComponent() override;

    void Render(const Timing& timing);

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] const Matrix<4, 4>& Projection();

    [[nodiscard]] std::uint32_t ViewWidth() const { return viewWidth; }
    [[nodiscard]] std::uint32_t ViewHeight() const { return viewHeight; }

    [[nodiscard]] const Graphic::GL::ColorFloat& GetClearColor() const { return clearColor; }

    [[nodiscard]] GraphicAPIWindowType& GetNativeWindow() const
    {
      Expects(window != nullptr);
      return *window;
    }

    [[nodiscard]] gsl::not_null<const Graphic::GL::Framebuffer*> GetFramebuffer() const { return framebuffer; }

  private:
    static inline CameraComponent* current;

    std::uint32_t viewWidth{};
    std::uint32_t viewHeight{};
    float height{};
    float nearClip{};
    float farClip{};
    Graphic::GL::ColorFloat clearColor{};
    GraphicAPIWindowType* window{};
    Graphic::GL::Framebuffer* framebuffer{};
    Graphic::RenderTextureRef renderTexture;
    Matrix<4, 4> projection{};
    std::vector<ComponentReference<CompositorComponent>> compositors;
  };
} // namespace SimpleGE
