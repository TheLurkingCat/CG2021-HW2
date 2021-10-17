#pragma once
#include <memory>
#include <vector>

#include <glad/gl.h>

#include "buffer/buffer.h"
#include "buffer/vertexarray.h"
#include "shape.h"

namespace graphics::shape {
class Plane final : public Shape {
 public:
  Plane();
  static void generateVertices(std::vector<GLfloat>& vertex, std::vector<GLuint>& index);
  void draw() const noexcept override;
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "Plane"; }
  static std::unique_ptr<Plane> make_unique();

 private:
  std::shared_ptr<buffer::VertexArray> vao;
  std::shared_ptr<buffer::ArrayBuffer> vbo;
  std::shared_ptr<buffer::ElementArrayBuffer> ebo;

  static std::weak_ptr<buffer::VertexArray> vao_weak;
  static std::weak_ptr<buffer::ArrayBuffer> vbo_weak;
  static std::weak_ptr<buffer::ElementArrayBuffer> ebo_weak;
};
using PlanePTR = std::unique_ptr<Plane>;
};  // namespace graphics::shape
