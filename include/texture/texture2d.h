#pragma once
#include "texture/texture.h"

#include <glm/fwd.hpp>

#include "filesystem.h"
namespace graphics::texture {
class Texture2D : public Texture {
 public:
  MOVE_ONLY(Texture2D)
  Texture2D() noexcept;
  void fromFile(const utils::fs::path& path);
  void fromColor(const glm::vec4& color);
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "Texture2D"; }
  CONSTEXPR_VIRTUAL GLenum getType() const noexcept override { return GL_TEXTURE_2D; }
};
}  // namespace graphics::texture
