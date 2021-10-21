#pragma once
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"

namespace graphics::light {
enum class LightType : uint8_t { Point, Directional, Spot };
class Light {
 public:
  explicit Light(const glm::vec4& lightvector) noexcept : lightVector(lightvector) {}
  virtual ~Light() = default;
  void setLightSpaceMatrix(const glm::mat4& mat4) { lightSpaceMatrix = mat4; }
  glm::mat4 getLightSpaceMatrix() const { return lightSpaceMatrix; }
  const float* getLightSpaceMatrixPTR() const { return glm::value_ptr(lightSpaceMatrix); }
  void setLightVector(const glm::vec4& vec4) { lightVector = vec4; }
  glm::vec4 getLightVector() const { return lightVector; }
  const float* getLightVectorPTR() const { return glm::value_ptr(lightVector); }
  CONSTEXPR_VIRTUAL virtual const char* getTypeName() const = 0;
  CONSTEXPR_VIRTUAL virtual LightType getType() const = 0;

 protected:
  glm::vec4 lightVector;
  glm::mat4 lightSpaceMatrix;
};
using LightPTR = std::unique_ptr<Light>;
}  // namespace graphics::light
