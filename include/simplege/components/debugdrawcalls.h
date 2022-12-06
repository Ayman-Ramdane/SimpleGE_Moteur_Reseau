#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/logic.h>

namespace SimpleGE
{
  class DebugDrawCallsComponent : public LogicComponent
  {
  public:
    static constexpr auto Type = "DebugDrawCalls";

    DebugDrawCallsComponent(Entity& owner);
    ~DebugDrawCallsComponent() override;

    void UpdateLogic(const Timing& timing) override;

  private:
    int calls{};
    int frames{};
    std::chrono::milliseconds elapsed{};
  };
} // namespace SimpleGE
