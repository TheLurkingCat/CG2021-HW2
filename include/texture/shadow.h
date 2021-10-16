#pragma once
#include "texture/texture.h"

namespace graphics::texture {

class FrameBuffer {
 public:
  MOVE_ONLY(FrameBuffer)
  FrameBuffer() noexcept;
  ~FrameBuffer();

  void bind() const;

 private:
  GLuint handle;
};

class ShadowMap final : public Texture {
 public:
  MOVE_ONLY(ShadowMap)
  ShadowMap(unsigned int size);
  unsigned int getSize() const { return shadowSize; }
  void bindFrameBuffer() const noexcept { framebuffer.bind(); }
  CONSTEXPR_VIRTUAL const char* getTypeName() const noexcept override { return "Texture2D (shadow map)"; }
  CONSTEXPR_VIRTUAL GLenum getType() const noexcept override { return GL_TEXTURE_2D; }

 private:
  FrameBuffer framebuffer;
  const unsigned int shadowSize;
};
}  // namespace graphics::texture
