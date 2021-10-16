#include "shape/plane.h"

namespace graphics::shape {

std::weak_ptr<buffer::VertexArray> Plane::vao_weak;
std::weak_ptr<buffer::ArrayBuffer> Plane::vbo_weak;
std::weak_ptr<buffer::ElementArrayBuffer> Plane::ebo_weak;

Plane::Plane() {
  if ((ebo = ebo_weak.lock())) {
    // Already have vertex data.
    vao = vao_weak.lock();
    vbo = vbo_weak.lock();
  } else {
    // First object.
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    generateVertices(vertices, indices);

    vao_weak = vao = std::make_shared<buffer::VertexArray>();
    vbo_weak = vbo = std::make_shared<buffer::ArrayBuffer>();
    ebo_weak = ebo = std::make_shared<buffer::ElementArrayBuffer>();

    vbo->allocate_load(vertices.size() * sizeof(GLfloat), vertices.data());
    ebo->allocate_load(indices.size() * sizeof(GLuint), indices.data());

    vao->bind();
    vbo->bind();
    ebo->bind();

    vao->enable(0);
    vao->setAttributePointer(0, 3, 8, 0);
    vao->enable(1);
    vao->setAttributePointer(1, 3, 8, 3);
    vao->enable(2);
    vao->setAttributePointer(2, 2, 8, 6);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}

void Plane::draw() const noexcept {
  vao->bind();
  glDrawElements(GL_TRIANGLE_STRIP, ebo->getSize() / sizeof(GLuint), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void Plane::generateVertices(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
  vertices = std::vector<GLfloat>{-1, 0, 1,  0, 1, 0, 0, 0, 1, 0, 1,  0, 1, 0, 1, 0,
                                  -1, 0, -1, 0, 1, 0, 0, 1, 1, 0, -1, 0, 1, 0, 1, 1};
  indices = std::vector<GLuint>{0, 1, 2, 3};
}
}  // namespace graphics::shape
