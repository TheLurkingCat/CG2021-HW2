#pragma once
#include <vector>

#include <glad/gl.h>

#include "utils.h"
namespace graphics::buffer {
class Buffer {
 public:
  MOVE_ONLY(Buffer)
  Buffer() noexcept;
  virtual ~Buffer();

  void bind() const noexcept;
  void allocate(GLsizeiptr _size, GLenum usage = GL_STATIC_DRAW) noexcept;
  void load(GLintptr offset, GLsizeiptr _size, const void* data) noexcept;
  void allocate_load(GLsizeiptr _size, const void* data, GLenum usage = GL_STATIC_DRAW) noexcept;

  CONSTEXPR_VIRTUAL virtual const char* getTypeName() const noexcept = 0;
  CONSTEXPR_VIRTUAL virtual GLenum getType() const noexcept = 0;
  GLuint getHandle() const noexcept { return handle; }
  GLuint getSize() const noexcept { return size; }

 private:
  GLuint handle;
  GLuint size;
};

class ArrayBuffer final : public Buffer {
 public:
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "Array buffer"; };
  CONSTEXPR_VIRTUAL GLenum getType() const noexcept override { return GL_ARRAY_BUFFER; };
};

class ElementArrayBuffer final : public Buffer {
 public:
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "Element array buffer"; };
  CONSTEXPR_VIRTUAL GLenum getType() const noexcept override { return GL_ELEMENT_ARRAY_BUFFER; };
};
}  // namespace graphics::buffer
