#pragma once
#include "utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace graphics::shape {
class Shape {
 public:
  Shape() noexcept : modelMatrix(glm::identity<glm::mat4>()) {}
  virtual ~Shape() = default;
  virtual void bindvbo() const noexcept = 0;
  virtual void draw() const noexcept = 0;
  CONSTEXPR_VIRTUAL virtual const char* getTypeName() const noexcept = 0;

  void setModelMatrix(const glm::mat4& _modelMatrix) noexcept { modelMatrix = _modelMatrix; }
  glm::mat4 getModelMatrix() const noexcept { return modelMatrix; }

 private:
  glm::mat4 modelMatrix;
};
}  // namespace graphics::shape
