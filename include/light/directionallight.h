#pragma once
#include "light.h"
namespace graphics::light {
class DirectionalLight : public Light {
 public:
  explicit DirectionalLight(const glm::vec3& lightVector) noexcept;
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "Directional light"; }
  static std::unique_ptr<DirectionalLight> make_unique(const glm::vec3& lightVector);
};
using DirectionalLightPTR = std::unique_ptr<DirectionalLight>;
}  // namespace graphics::light
