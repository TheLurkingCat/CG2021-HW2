#include "light/pointlight.h"
#include <utility>

#include <glm/gtc/matrix_transform.hpp>

namespace graphics::light {
PointLight::PointLight(const glm::vec3& lightVector) noexcept :
    Light(glm::vec4(lightVector, 1), glm::vec4(0, 0, 0, 1)) {
  glm::mat4 viewProjection(1);
  viewProjection *= glm::perspective(glm::half_pi<float>(), 1.0f, 1.0f, 100.0f);
  viewProjection *= glm::lookAt(lightVector, glm::vec3(0), glm::vec3(0, 1, 0));
  setLightSpaceMatrix(viewProjection);
}
PointLightPTR PointLight::make_unique(const glm::vec3& lightVector) {
  return std::make_unique<PointLight>(lightVector);
}
}  // namespace graphics::light
