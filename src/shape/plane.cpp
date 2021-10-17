#include "shape/plane.h"
namespace {
constexpr int g_Subdivision = 20;
}  // namespace
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

PlanePTR Plane::make_unique() { return std::make_unique<Plane>(); }

void Plane::generateVertices(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
  vertices.reserve((g_Subdivision + 1) * (g_Subdivision + 1) * 8);
  indices.reserve((g_Subdivision) * (2 * g_Subdivision + 3));
  float step = 2.0f / g_Subdivision;
  float textureStep = 1.0f / g_Subdivision;
  for (int i = 0; i < g_Subdivision + 1; ++i) {
    for (int j = 0; j < g_Subdivision + 1; ++j) {
      vertices.insert(vertices.end(),
                      {-1 + j * step, 0, -1 + i * step, 0, 1, 0, 0 + j * textureStep, 1 - i * textureStep});
    }
  }

  for (int i = 0; i < g_Subdivision; ++i) {
    int offset = i * (g_Subdivision + 1);
    for (int j = 0; j < g_Subdivision + 1; ++j) {
      indices.emplace_back(offset + j);
      indices.emplace_back(offset + j + g_Subdivision + 1);
    }
    // Primitive restart
    indices.emplace_back(65535);
  }
}
}  // namespace graphics::shape
