#include "texture/texture2d.h"
#include <cstdint>
#include <fstream>

#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

namespace graphics::texture {

Texture2D::Texture2D() noexcept {}

void Texture2D::fromFile(const std::filesystem::path& filename) {
  int width, height, nChannels;
  stbi_set_flip_vertically_on_load(1);
  stbi_uc* data = stbi_load(filename.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
  if (data == nullptr) THROW_EXCEPTION(std::runtime_error, "Failed to load texture file");
  bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void Texture2D::fromColor(const glm::vec4& color) {
  glm::u8vec4 colorByte = glm::round(255.0f * color);
  bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, glm::value_ptr(colorByte));
}
}  // namespace graphics::texture