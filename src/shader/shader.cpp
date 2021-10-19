#include "shader/shader.h"

#include <fstream>
namespace {
std::string readFile(const utils::fs::path& filename) {
  std::ifstream shaderFile(filename);
  if (!shaderFile) {
    THROW_EXCEPTION(std::runtime_error, "Failed to open shader file!");
  }
  auto shaderCode = std::string(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());
  shaderFile.close();
  return shaderCode;
}
}  // namespace

namespace graphics::shader {

Shader::Shader(GLenum shaderType) noexcept :
    handle(glCreateShader(shaderType)) {}
Shader::~Shader() { glDeleteShader(handle); }

void Shader::fromFile(const utils::fs::path& filename) {
  std::string shaderCode = readFile(filename);
  this->fromString(shaderCode);
}

void Shader::fromString(const std::string& shaderCode) {
  auto shaderCodePointer = shaderCode.c_str();
  glShaderSource(handle, 1, &shaderCodePointer, nullptr);
  glCompileShader(handle);
  GLint success;
  GLchar infoLog[1024];
  glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(handle, 1024, nullptr, infoLog);
    puts("Shader compilation error!");
    puts(infoLog);
  }
}
}  // namespace graphics::shader
