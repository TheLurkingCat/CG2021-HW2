#include "light/directionallight.h"
#include <glm/gtc/matrix_transform.hpp>

namespace graphics::light {
DirectionalLight::DirectionalLight(const glm::vec3& lightVector) noexcept : Light(glm::vec4(lightVector, 0)) {
  glm::mat4 viewProjection(1);
  viewProjection *= glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, -75.0f, 75.0f);
  viewProjection *= glm::lookAt(lightVector, glm::vec3(0), glm::vec3(0, 1, 0));
  setLightSpaceMatrix(viewProjection);
}
DirectionalLightPTR DirectionalLight::make_unique(const glm::vec3& lightVector) {
  return std::make_unique<DirectionalLight>(lightVector);
}
}  // namespace graphics::light
