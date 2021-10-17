#pragma once
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
namespace graphics::shape {
class Shape {
 public:
  Shape() noexcept : modelMatrix(1), normalMatrix(1) {}
  virtual ~Shape() = default;
  virtual void draw() const noexcept = 0;
  CONSTEXPR_VIRTUAL virtual const char* getTypeName() const noexcept = 0;

  void setModelMatrix(const glm::mat4& _modelMatrix) noexcept {
    modelMatrix = _modelMatrix;
    normalMatrix = glm::mat4(glm::inverseTranspose(glm::mat3(modelMatrix)));
  }

  glm::mat4 getModelMatrix() const noexcept { return modelMatrix; }
  const float* getModelMatrixPTR() const noexcept { return glm::value_ptr(modelMatrix); }
  glm::mat4 getNormalMatrix() const noexcept { return normalMatrix; }
  const float* getNormalMatrixPTR() const noexcept { return glm::value_ptr(normalMatrix); }

 private:
  glm::mat4 modelMatrix;
  glm::mat4 normalMatrix;
};
using ShapePTR = std::unique_ptr<Shape>;
}  // namespace graphics::shape
