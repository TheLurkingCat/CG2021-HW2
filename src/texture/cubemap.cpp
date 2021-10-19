#include "texture/cubemap.h"
#include <array>

#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>
namespace graphics::texture {
TextureCubeMap::TextureCubeMap() noexcept {}

void TextureCubeMap::fromFile(const utils::fs::path& posx,
                              const utils::fs::path& negx,
                              const utils::fs::path& posy,
                              const utils::fs::path& negy,
                              const utils::fs::path& posz,
                              const utils::fs::path& negz) {
  std::array<const utils::fs::path, 6> filenames{posx, negx, posy, negy, posz, negz};
  int width, height, nChannels;
  stbi_set_flip_vertically_on_load(1);
  bind();
  for (int i = 0; i < 6; ++i) {
    stbi_uc* data = stbi_load(filenames[i].string().c_str(), &width, &height, &nChannels, STBI_default);
    GLenum format = GL_RGBA;
    switch (nChannels) {
      case 1:
        format = GL_RED;
        break;
      case 2:
        format = GL_RG;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4:
        format = GL_RGBA;
        break;
      default:
        THROW_EXCEPTION(std::runtime_error, "Unknown ");
    }
    if (data == nullptr) THROW_EXCEPTION(std::runtime_error, "Failed to load texture file");
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void TextureCubeMap::fromColor(const glm::vec4& posx,
                               const glm::vec4& negx,
                               const glm::vec4& posy,
                               const glm::vec4& negy,
                               const glm::vec4& posz,
                               const glm::vec4& negz) {
  std::array<const glm::vec4, 6> colors{posx, negx, posy, negy, posz, negz};
  bind();
  for (int i = 0; i < 6; ++i) {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, glm::value_ptr(colors[i]));
  }
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

}  // namespace graphics::texture
