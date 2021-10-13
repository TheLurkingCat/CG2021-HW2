#include <algorithm>
#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>

#include "buffer/vertexarray.h"
#include "camera/quat_camera.h"
#include "context_manager.h"
#include "shader/program.h"
#include "shader/shader.h"
#include "shape/sphere.h"

// Unnamed namespace for global variables
namespace {}  // namespace

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold
  if (action != GLFW_PRESS) return;
  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
}

void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<graphics::camera::Camera*>(glfwGetWindowUserPointer(window));
  if (ptr != nullptr) {
    ptr->updateProjection(OpenGLContext::getAspectRatio());
  }
}

int main() {
  // Initialize OpenGL context, details are wrapped in class.
  OpenGLContext::createContext(43, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow* window = OpenGLContext::getWindow();
  glfwSetWindowTitle(window, "HW2");
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
  // Initialize shader
  graphics::shader::ShaderProgram p;
  {
    graphics::shader::VertexShader vs;
    graphics::shader::FragmentShader fs;
    vs.fromFile("assets/shader/phong.vert");
    fs.fromFile("assets/shader/phong.frag");
    p.attachLinkDetachShader(vs, fs);
    p.bind();

    p.setUniform("baseColor", glm::vec4(1, 0.5, 0, 1));
    p.setUniform("lightPos", glm::vec4(0, 5, 5, 1));
  }
  // Setup camera.
  graphics::camera::QuaternionCamera camera(glm::vec3(0, 0, 15));
  camera.initialize(OpenGLContext::getAspectRatio());
  glfwSetWindowUserPointer(window, &camera);
  // Objects
  graphics::shape::Sphere sphere;
  // Setup VAO
  graphics::buffer::VertexArray vao;
  vao.bind();
  sphere.bindvbo();
  vao.enable(0);
  vao.setAttributePointer(0, 3, 8, 0);
  vao.enable(1);
  vao.setAttributePointer(1, 3, 8, 3);
  vao.enable(2);
  vao.setAttributePointer(2, 2, 8, 6);
  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    camera.move(window);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Update proj and view
    glm::mat4 model = sphere.getModelMatrix();
    p.setUniform("model", model);
    p.setUniform("VP", camera.getViewProjectionMatrix());
    p.setUniform("invtransmodel", glm::inverse(glm::transpose(model)));
    p.setUniform("viewPos", camera.getPosition());
    sphere.draw();
#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}
