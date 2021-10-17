#pragma once
#include "light.h"
namespace graphics ::light {
class PointLight final : public Light {
 public:
  explicit PointLight(const glm::vec3& lightVector) noexcept;
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "Point light"; }
  static std::unique_ptr<PointLight> make_unique(const glm::vec3& lightVector);
};
using PointLightPTR = std::unique_ptr<PointLight>;
}  // namespace graphics::light
