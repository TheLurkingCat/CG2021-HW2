#include "shape/sphere.h"
namespace {
constexpr int g_SphereSectors = 36;
constexpr int g_SphereStacks = 18;
}  // namespace

namespace graphics::shape {

std::weak_ptr<buffer::VertexArray> Sphere::vao_weak;
std::weak_ptr<buffer::ArrayBuffer> Sphere::vbo_weak;
std::weak_ptr<buffer::ElementArrayBuffer> Sphere::ebo_weak;

Sphere::Sphere() {
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

void Sphere::draw() const noexcept {
  vao->bind();
  glDrawElements(GL_TRIANGLES, ebo->getSize() / sizeof(GLuint), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void Sphere::generateVertices(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
  // http://www.songho.ca/opengl/gl_sphere.html#sphere
  vertices.reserve(8 * (g_SphereStacks + 1) * (g_SphereSectors + 1));
  indices.reserve(6 * g_SphereStacks * g_SphereSectors);

  float x, y, z, xy;  //  position
  float s, t;         //  texCoord

  constexpr float sectorStep = glm::two_pi<float>() / g_SphereSectors;
  constexpr float stackStep = glm::pi<float>() / g_SphereStacks;
  float sectorAngle, stackAngle;

  for (int i = 0; i <= g_SphereStacks; ++i) {
    stackAngle = static_cast<float>(glm::half_pi<float>() - i * stackStep);  // [pi/2, -pi/2]
    xy = cosf(stackAngle);                                                   // r * cos(u)
    z = sinf(stackAngle);                                                    // r * sin(u)

    for (int j = 0; j <= g_SphereSectors; ++j) {
      sectorAngle = j * sectorStep;  // [0, 2pi]

      x = xy * cosf(sectorAngle);  // r * cos(u) * cos(v)
      y = xy * sinf(sectorAngle);  // r * cos(u) * sin(v)

      // vertex tex coord (s, t) range between [0, 1]
      s = static_cast<float>(j) / g_SphereSectors;
      t = static_cast<float>(i) / g_SphereStacks;

      vertices.insert(vertices.end(), {x, y, z, x, y, z, s, t});
    }
  }

  unsigned int k1, k2;  // EBO index
  for (int i = 0; i < g_SphereStacks; ++i) {
    k1 = i * (g_SphereSectors + 1);  // beginning of current stack
    k2 = k1 + g_SphereSectors + 1;   // beginning of next stack
    for (int j = 0; j < g_SphereSectors; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.insert(indices.end(), {k1, k2, k1 + 1});
      }
      // k1+1 => k2 => k2+1
      if (i != (g_SphereStacks - 1)) {
        indices.insert(indices.end(), {k1 + 1, k2, k2 + 1});
      }
    }
  }
}
}  // namespace graphics::shape
