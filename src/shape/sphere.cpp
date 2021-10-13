#include "shape/sphere.h"
namespace {
constexpr int g_SphereSectors = 360;
constexpr int g_SphereStacks = 180;
}  // namespace

namespace graphics::shape {

std::weak_ptr<buffer::ArrayBuffer> Sphere::vbo_weak;
std::weak_ptr<buffer::ElementArrayBuffer> Sphere::ebo_weak;

Sphere::Sphere() {
  if ((ebo = ebo_weak.lock())) {
    // Already have vertex data.
    vbo = vbo_weak.lock();
  } else {
    // First object.
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    generateVertices(vertices, indices);

    vbo = std::make_shared<buffer::ArrayBuffer>();
    ebo = std::make_shared<buffer::ElementArrayBuffer>();

    vbo->allocate_load(vertices);
    ebo->allocate_load(indices);

    vbo_weak = vbo;
    ebo_weak = ebo;
  }
}
void Sphere::bindvbo() const noexcept { vbo->bind(); }

void Sphere::draw() const noexcept {
  vbo->bind();
  ebo->bind();
  glDrawElements(GL_TRIANGLES, ebo->size() / sizeof(GLuint), GL_UNSIGNED_INT, nullptr);
}

void Sphere::generateVertices(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
  // http://www.songho.ca/opengl/gl_sphere.html#sphere
  vertices.reserve(8 * (g_SphereStacks + 1) * (g_SphereSectors + 1));
  indices.reserve(6 * g_SphereStacks * g_SphereSectors);

  float x, y, z, xy;  //  position
  float s, t;         //  texCoord

  constexpr float sectorStep = 2.0f * utils::PI<float>() / g_SphereSectors;
  constexpr float stackStep = utils::PI<float>() / g_SphereStacks;
  float sectorAngle, stackAngle;

  for (int i = 0; i <= g_SphereStacks; ++i) {
    stackAngle = static_cast<float>(utils::PI<float>() / 2.0 - i * stackStep);  // [pi/2, -pi/2]
    xy = cosf(stackAngle);                                                      // r * cos(u)
    z = sinf(stackAngle);                                                       // r * sin(u)

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
