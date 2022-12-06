#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/logic.h>
#include <simplege/components/spritesheet.h>
#include <simplege/components/visual.h>

#include <simplege/eventtrigger.h>

namespace SimpleGE
{
  class SpriteSheetComponent;

  class SpriteComponent : public LogicComponent, public VisualComponent
  {
  public:
    static constexpr auto Type = "Sprite";

    using AnimationEndedEventType = std::function<void()>;

    struct Description
    {
      int frameSkip{1};
      bool isAnimated{};
      int animWait{};
      std::string spriteName{};
      ComponentReference<SpriteSheetComponent> spriteSheet;
    };

    SpriteComponent(Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const Timing& timing) override;
    void Display(const Timing& timing) override;

    void SetFrameSkip(int val) { frameSkip = val; }
    void SetAnimationFrame(int val) { animationFrame = val; }
    void SetIsAnimated(bool val) { isAnimated = val; }
    void UpdateMesh();

    void SetSpriteName(std::string_view spriteName);

    void RegisterAnimationEndedEvent(const AnimationEndedEventType& onAnimationEnded)
    {
      animationEndedEvent.Register(onAnimationEnded);
    }

    [[nodiscard]] ComponentReference<SpriteSheetComponent> GetSpriteSheet() const { return spriteSheet; }

  private:
    std::string FindNextFrameName();
    void UpdateComponents();

    struct Vertex
    {
      float x, y, z;
      float u, v;
    };

    ComponentReference<SpriteSheetComponent> spriteSheet;
    std::string spriteName;
    EventTrigger<AnimationEndedEventType> animationEndedEvent;
    bool isAnimated{};
    int frameSkip{};
    int animationFrame{};
    int animWait{};
    int animWaitCounter{};
    const SpriteSheetComponent::Frame* descr{};
    Size<int> spriteSize{};
    Graphic::VertexBufferRef<Vertex, Graphic::BufferUsage::Dynamic> vertexBuffer;
    std::array<Vertex, 4> vertices{};
    static constexpr std::array<std::uint16_t, 6> Indices{0, 1, 2, 2, 3, 0};
    Graphic::IndexBufferRef<std::uint16_t, Graphic::BufferUsage::Dynamic> indexBuffer;
  };
} // namespace SimpleGE
