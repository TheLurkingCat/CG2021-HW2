#include "shader/program.h"

#include <glm/gtc/type_ptr.hpp>

namespace graphics::shader {
ShaderProgram::ShaderProgram() noexcept : handle(0) { handle = glCreateProgram(); }
ShaderProgram::~ShaderProgram() { glDeleteProgram(handle); }

void ShaderProgram::link() {
  glLinkProgram(handle);
  GLint success;
  GLchar infoLog[1024];
  glGetProgramiv(handle, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(handle, 1024, nullptr, infoLog);
    puts("Failed to link shader program!");
    puts(infoLog);
  }
}
void ShaderProgram::bind() const noexcept { glUseProgram(handle); }
void ShaderProgram::bindUniformBlock(const char* name, GLuint index) const noexcept {
  glUniformBlockBinding(handle, getUniformBlockIndex(name), index);
}
void ShaderProgram::setUniform(const char* name, int i1) { glUniform1i(getUniformLocation(name), i1); }
void ShaderProgram::setUniform(const char* name, const glm::mat4& mat4) {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4));
}
void ShaderProgram::setUniform(const char* name, const glm::vec3& vec3) {
  glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(vec3));
}
void ShaderProgram::setUniform(const char* name, const glm::vec4& vec4) {
  glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(vec4));
}
}  // namespace graphics::shader
