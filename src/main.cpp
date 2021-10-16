#include <algorithm>
#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION
#include "graphics.h"

// Unnamed namespace for global variables
namespace {
// Lights
graphics::light::Light* currentLight;
graphics::light::PointLight* pLight;
graphics::light::DirectionalLight* dirLight;
}  // namespace

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold
  if (action != GLFW_PRESS) return;
  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  switch (key) {
    case GLFW_KEY_H:
      currentLight = dirLight;
      break;
    case GLFW_KEY_P:
      currentLight = pLight;
      break;
    default:
      break;
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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
  // Initialize shader
  graphics::shader::ShaderProgram phongShader, gouraudShader, shadowShader;
  {
    graphics::shader::VertexShader vs;
    graphics::shader::FragmentShader fs;
    vs.fromFile("../assets/shader/phong.vert");
    fs.fromFile("../assets/shader/phong.frag");
    phongShader.attachLinkDetachShader(vs, fs);
    phongShader.bind();
    phongShader.setUniform("diffuseTexture", 0);
    phongShader.setUniform("shadowMap", 1);
  }
  {
    graphics::shader::VertexShader vs;
    graphics::shader::FragmentShader fs;
    vs.fromFile("../assets/shader/gouraud.vert");
    fs.fromFile("../assets/shader/gouraud.frag");
    gouraudShader.attachLinkDetachShader(vs, fs);
    gouraudShader.bind();
    gouraudShader.setUniform("diffuseTexture", 0);
    gouraudShader.setUniform("shadowMap", 1);
  }
  {
    graphics::shader::VertexShader vs;
    graphics::shader::FragmentShader fs;
    vs.fromFile("../assets/shader/shadow.vert");
    fs.fromFile("../assets/shader/shadow.frag");
    shadowShader.attachLinkDetachShader(vs, fs);
    shadowShader.bind();
  }
  // Texture
  graphics::texture::Texture2D baseColor;
  graphics::texture::ShadowMap shadow(1024);
  baseColor.fromColor(glm::vec4(1, 0.5, 0, 1));
  baseColor.bind(0);
  shadow.bind(1);
  // Setup camera.
  graphics::camera::QuaternionCamera camera(glm::vec3(0, 0, 15));
  camera.initialize(OpenGLContext::getAspectRatio());
  glfwSetWindowUserPointer(window, &camera);
  // Setup lights
  graphics::light::PointLight pointlight(glm::vec3(0, 8, 6));
  graphics::light::DirectionalLight directionallight(glm::vec3(0, 8, 6));
  pLight = &pointlight;
  dirLight = &directionallight;
  currentLight = pLight;

  // Objects
  graphics::shape::Sphere phongSphere, gouraudSphere;
  glm::mat4 model = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(1, 0, 0));
  model = glm::scale(model, glm::vec3(3));
  phongSphere.setModelMatrix(glm::translate(model, glm::vec3(-1.5, 0, 0)));
  gouraudSphere.setModelMatrix(glm::translate(model, glm::vec3(1.5, 0, 0)));

  graphics::shape::Plane plane;
  model = glm::scale(glm::mat4(1), glm::vec3(20, 1, 20));
  model = glm::translate(model, glm::vec3(0, -3.5, 0));
  plane.setModelMatrix(model);
  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    camera.move(window);
    // Render shadow first
    glViewport(0, 0, shadow.getSize(), shadow.getSize());
    glCullFace(GL_FRONT);
    shadowShader.bind();
    shadow.bindFrameBuffer();
    glClear(GL_DEPTH_BUFFER_BIT);
    shadowShader.setUniform("model", gouraudSphere.getModelMatrix());
    shadowShader.setUniform("lightSpaceMatrix", currentLight->getLightSpaceMatrix());
    gouraudSphere.draw();
    shadowShader.setUniform("model", phongSphere.getModelMatrix());
    phongSphere.draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    glViewport(0, 0, OpenGLContext::getWidth(), OpenGLContext::getHeight());
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Render gouraud shading sphere
    glm::mat4 modelMatrix = gouraudSphere.getModelMatrix();
    gouraudShader.bind();
    gouraudShader.setUniform("model", modelMatrix);
    gouraudShader.setUniform("viewProjection", camera.getViewProjectionMatrix());
    gouraudShader.setUniform("normalMatrix", glm::inverse(glm::transpose(modelMatrix)));
    gouraudShader.setUniform("viewPosition", camera.getPosition());
    gouraudShader.setUniform("lightVector", currentLight->getLightVector());
    gouraudShader.setUniform("lightSpaceMatrix", currentLight->getLightSpaceMatrix());
    gouraudSphere.draw();
    // Render phong shading sphere
    modelMatrix = phongSphere.getModelMatrix();
    phongShader.bind();
    phongShader.setUniform("model", modelMatrix);
    phongShader.setUniform("viewProjection", camera.getViewProjectionMatrix());
    phongShader.setUniform("normalMatrix", glm::inverse(glm::transpose(modelMatrix)));
    phongShader.setUniform("viewPosition", camera.getPosition());
    phongShader.setUniform("lightVector", currentLight->getLightVector());
    phongShader.setUniform("lightSpaceMatrix", currentLight->getLightSpaceMatrix());
    phongSphere.draw();
    modelMatrix = plane.getModelMatrix();
    phongShader.setUniform("model", modelMatrix);
    phongShader.setUniform("normalMatrix", glm::inverse(glm::transpose(modelMatrix)));
    plane.draw();

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}
