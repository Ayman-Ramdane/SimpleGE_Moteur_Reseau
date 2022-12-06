#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/input.h>

namespace SimpleGE
{
  class KeyboardInputComponent : public InputComponent
  {
  public:
    using KeyAPIType = int;

    static constexpr auto Type = "KeyboardInput";

    struct Description
    {
      std::unordered_map<Action, std::string> mapping;
    };

    KeyboardInputComponent(Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] bool IsActive(Action action) const override;

  private:
    std::unordered_map<InputComponent::Action, KeyAPIType> mapping;
  };
} // namespace SimpleGE
