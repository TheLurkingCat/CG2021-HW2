#include "buffer/buffer.h"

namespace graphics::buffer {
Buffer::Buffer() noexcept : handle(0), size(0) { glGenBuffers(1, &handle); }
Buffer::~Buffer() { glDeleteBuffers(1, &handle); }
void Buffer::bind() const noexcept { glBindBuffer(getType(), handle); }

void Buffer::allocate(GLsizeiptr _size, GLenum usage) noexcept {
  bind();
  size = _size;
  glBufferData(getType(), size, nullptr, usage);
}

void Buffer::load(GLintptr offset, GLsizeiptr _size, const void* data) noexcept {
  bind();
  glBufferSubData(getType(), offset, _size, data);
}
void Buffer::allocate_load(GLsizeiptr _size, const void* data, GLenum usage) noexcept {
  bind();
  size = _size;
  glBufferData(getType(), size, data, usage);
}
}  // namespace graphics::buffer
