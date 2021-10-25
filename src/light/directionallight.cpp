#include "light/directionallight.h"
#include <glm/gtc/matrix_transform.hpp>

namespace graphics::light {
DirectionalLight::DirectionalLight(const glm::vec3& lightVector) noexcept :
    Light(glm::vec4(lightVector, 0), glm::vec4(0, 0, 0, 0)) {
  glm::mat4 viewProjection(1);
  // TODO: Set view projection matrix
  // Hint:
  //	   1. zNear, zFar, bottom, top, left, right = -75.0, 75.0, -30.0, 30.0, -30.0, 30.0
  //	   glm::ortho (https://glm.g-truc.net/0.9.9/api/a00665.html#ga6615d8a9d39432e279c4575313ecb456)
  // Note: You should not use glOrtho
  viewProjection *= glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, -75.0f, 75.0f);
  viewProjection *= glm::lookAt(lightVector, glm::vec3(0), glm::vec3(0, 1, 0));
  setLightSpaceMatrix(viewProjection);
}
DirectionalLightPTR DirectionalLight::make_unique(const glm::vec3& lightVector) {
  return std::make_unique<DirectionalLight>(lightVector);
}
}  // namespace graphics::light
