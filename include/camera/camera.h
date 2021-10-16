#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace graphics::camera {

class Camera {
 public:
  explicit Camera(const glm::vec3& position);
  virtual ~Camera() = default;

  glm::vec4 getPosition() const { return glm::vec4(position, 1.0); }
  glm::mat4 getProjectionMatrix() const { return projectionMatrix; }
  glm::mat4 getViewMatrix() const { return viewMatrix; }
  glm::mat4 getViewProjectionMatrix() const { return projectionMatrix * viewMatrix; }
  void initialize(float aspectRatio);
  virtual void move(GLFWwindow* window) = 0;
  virtual void updateProjection(float aspectRatio) = 0;

 protected:
  virtual void updateView() = 0;
  glm::vec3 position;
  glm::vec3 up;
  glm::vec3 front;
  glm::vec3 right;
  glm::mat4 projectionMatrix;
  glm::mat4 viewMatrix;
};
}  // namespace graphics::camera
