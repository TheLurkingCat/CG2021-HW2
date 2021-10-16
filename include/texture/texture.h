#pragma once

#include <glad/gl.h>
#include "utils.h"

namespace graphics::texture {

class Texture {
 public:
  MOVE_ONLY(Texture)
  Texture() noexcept;
  virtual ~Texture();
  CONSTEXPR_VIRTUAL virtual const char* getTypeName() const noexcept = 0;
  CONSTEXPR_VIRTUAL virtual GLenum getType() const noexcept = 0;
  void bind(GLuint index = 0) const noexcept;

 protected:
  GLuint handle;
};
}  // namespace graphics::texture
