#pragma once
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"

namespace graphics::light {
class Light {
 public:
  Light(const glm::vec4& lightvector) noexcept : lightVector(lightvector) {}
  virtual ~Light() = default;
  void setLightSpaceMatrix(const glm::mat4& mat4) noexcept { lightSpaceMatrix = mat4; }
  glm::mat4 getLightSpaceMatrix() const noexcept { return lightSpaceMatrix; };
  const float* getLightSpaceMatrixPTR() const noexcept { return glm::value_ptr(lightSpaceMatrix); };
  void setLightVector(const glm::vec4& vec4) noexcept { lightVector = vec4; }
  glm::vec4 getLightVector() const noexcept { return lightVector; }
  const float* getLightVectorPTR() const noexcept { return glm::value_ptr(lightVector); };
  CONSTEXPR_VIRTUAL virtual const char* getTypeName() const noexcept = 0;

 protected:
  glm::vec4 lightVector;
  glm::mat4 lightSpaceMatrix;
};
using LightPTR = std::unique_ptr<Light>;
}  // namespace graphics::light
