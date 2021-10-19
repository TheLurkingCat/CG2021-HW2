#pragma once
#include "texture.h"

#include <glm/fwd.hpp>

#include "filesystem.h"

namespace graphics::texture {
class TextureCubeMap : public Texture {
 public:
  MOVE_ONLY(TextureCubeMap)
  TextureCubeMap() noexcept;
  void fromFile(const utils::fs::path& posx,
                const utils::fs::path& negx,
                const utils::fs::path& posy,
                const utils::fs::path& negy,
                const utils::fs::path& posz,
                const utils::fs::path& negz);

  void fromColor(const glm::vec4& posx,
                 const glm::vec4& negx,
                 const glm::vec4& posy,
                 const glm::vec4& negy,
                 const glm::vec4& posz,
                 const glm::vec4& negz);
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "TextureCubeMap"; }
  CONSTEXPR_VIRTUAL GLenum getType() const noexcept override { return GL_TEXTURE_CUBE_MAP; }
};
}  // namespace graphics::texture
