#pragma once
#include <vector>

#include <glad/gl.h>

#include "utils.h"
namespace graphics::buffer {
template <GLenum typeT, typename T>
class Buffer final {
 public:
  MOVE_ONLY(Buffer)
  Buffer() noexcept : handle(0), size_(0) { glGenBuffers(1, &handle); }
  ~Buffer() noexcept { glDeleteBuffers(1, &handle); }

  void allocate(GLsizeiptr size, GLenum usage = GL_STATIC_DRAW) noexcept {
    bind();
    size_ = size;
    glBufferData(typeT, size, nullptr, usage);
  }
  void load(const std::vector<T>& data, GLintptr offset = 0) {
    bind();
    glBufferSubData(typeT, offset, sizeof(T) * data.size(), data.data());
  }
  void allocate_load(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW) {
    bind();
    size_ = sizeof(T) * data.size();
    glBufferData(typeT, sizeof(T) * data.size(), data.data(), usage);
  }

  void bind() const noexcept { glBindBuffer(typeT, handle); }
  GLuint getHandle() const noexcept { return handle; }
  GLuint size() const noexcept { return size_; }

 private:
  GLuint handle;
  GLuint size_;
};
using ArrayBuffer = Buffer<GL_ARRAY_BUFFER, GLfloat>;
using ElementArrayBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER, GLuint>;

}  // namespace graphics::buffer
