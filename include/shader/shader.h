#pragma once
#include <string>

#include <glad/gl.h>

#include "filesystem.h"
#include "utils.h"
namespace graphics::shader {
class Shader {
 public:
  MOVE_ONLY(Shader)
  explicit Shader(GLenum shaderType) noexcept;
  virtual ~Shader();
  CONSTEXPR_VIRTUAL virtual const char* getTypeName() const noexcept = 0;
  CONSTEXPR_VIRTUAL virtual GLenum getType() const noexcept = 0;
  GLuint getHandle() const noexcept { return handle; }
  void fromFile(const utils::fs::path& filename);
  void fromString(const std::string& shadercode);

 protected:
  GLuint handle;
};

class VertexShader final : public Shader {
 public:
  MOVE_ONLY(VertexShader)
  VertexShader() noexcept : Shader(GL_VERTEX_SHADER) {}
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "Vertex shader"; }
  CONSTEXPR_VIRTUAL GLenum getType() const noexcept override { return GL_VERTEX_SHADER; }
};

class FragmentShader final : public Shader {
 public:
  MOVE_ONLY(FragmentShader)
  FragmentShader() noexcept : Shader(GL_FRAGMENT_SHADER) {}
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "Fragment shader"; }
  CONSTEXPR_VIRTUAL GLenum getType() const noexcept override { return GL_FRAGMENT_SHADER; }
};
}  // namespace graphics::shader
