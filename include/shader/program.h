#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>

#include "utils.h"
namespace graphics::shader {
class ShaderProgram final {
 public:
  MOVE_ONLY(ShaderProgram)
  ShaderProgram() noexcept;
  ~ShaderProgram();

  template <typename... Args>
  void attachShader(Args&&... args) {
    (glAttachShader(this->handle, args.getHandle()), ...);
  }
  void link();
  template <typename... Args>
  void detachShader(Args&&... args) {
    (glDetachShader(this->handle, args.getHandle()), ...);
  }

  template <typename... Args>
  void attachLinkDetachShader(Args&&... args) {
    (glAttachShader(this->handle, args.getHandle()), ...);
    link();
    (glDetachShader(this->handle, args.getHandle()), ...);
  }

  GLuint getHandle() const noexcept { return handle; }
  void bind() const noexcept;

  int getUniformLocation(const char* name) const noexcept { return glGetUniformLocation(handle, name); }
  void setUniform(const char* name, int i1);
  void setUniform(const char* name, const glm::mat4& mat4);
  void setUniform(const char* name, const glm::vec3& vec3);
  void setUniform(const char* name, const glm::vec4& vec4);

 private:
  GLuint handle;
};
}  // namespace graphics::shader
