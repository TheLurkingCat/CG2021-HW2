#pragma once
#include <memory>
#include "light.h"
namespace graphics::light {
class Spotlight final : public Light {
 public:
  explicit Spotlight(const glm::vec3& lightVector) noexcept;
  CONSTEXPR_VIRTUAL const char* getTypeName() const override { return "Spotlight"; }
  CONSTEXPR_VIRTUAL LightType getType() const override { return LightType::Spot; }
  static std::unique_ptr<Spotlight> make_unique(const glm::vec3& lightVector);
};
using SpotlightPTR = std::unique_ptr<Spotlight>;
}  // namespace graphics::light
