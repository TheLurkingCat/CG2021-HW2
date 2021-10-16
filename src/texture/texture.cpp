#include "texture/texture.h"

namespace graphics::texture {
Texture::Texture() noexcept : handle(0) { glGenTextures(1, &handle); }
Texture::~Texture() { glDeleteTextures(1, &handle); }
void Texture::bind(GLuint index) const noexcept {
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(getType(), handle);
}
}  // namespace graphics::texture
