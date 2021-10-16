#pragma once
#include "light.h"
namespace graphics::light {
class PointLight final : public Light {
 public:
  explicit PointLight(const glm::vec3& lightVector) noexcept;
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "Point light"; }
};
}  // namespace graphics::light
