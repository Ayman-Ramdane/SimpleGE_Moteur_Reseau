#include <simplege/simplege.h>

namespace SimpleGE
{
  using namespace std::chrono_literals;

  static constexpr auto DebugInterval = 5s;

  DebugDrawCallsComponent::DebugDrawCallsComponent(Entity& owner) : Component(owner)
  {
    Graphic::GL::DebugDrawCallHelper::Instance().onDraw = [this]() { calls++; };
  }

  DebugDrawCallsComponent::~DebugDrawCallsComponent() { Graphic::GL::DebugDrawCallHelper::Instance().onDraw = {}; }

  void DebugDrawCallsComponent::UpdateLogic(const Timing& timing)
  {
    frames++;
    elapsed += timing.delta;
    if (elapsed > DebugInterval)
    {
      float avgCalls = float(calls) / float(frames);
      fmt::print("Nb appels de rendu moyens par trame: {}\n", avgCalls);
      calls = 0;
      frames = 0;
      elapsed = {};
    }
  }
} // namespace SimpleGE
