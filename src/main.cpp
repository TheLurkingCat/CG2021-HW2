#include <algorithm>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

#include <iostream>

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
// Cameras
graphics::camera::Camera* currentCamera = nullptr;
// Control variables
bool isWindowSizeChanged = false;
bool isLightChanged = false;
int currentLight = 0;
int alignSize = 256;
// Configs
constexpr int LIGHT_COUNT = 2;
constexpr int CAMERA_COUNT = 1;
constexpr int MESH_COUNT = 3;
}  // namespace

int uboAlign(int i) { return ((i + 1 * (alignSize - 1)) / alignSize) * alignSize; }

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
      currentLight = 0;
      isLightChanged = true;
      break;
    case GLFW_KEY_P:
      currentLight = 1;
      isLightChanged = true;
      break;
    default:
      break;
  }
}

void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  assert(currentCamera != nullptr);
  currentCamera->updateProjection(OpenGLContext::getAspectRatio());
  isWindowSizeChanged = true;
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
    phongShader.bindUniformBlock("model", 0);
    phongShader.bindUniformBlock("camera", 1);
    phongShader.bindUniformBlock("light", 2);
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
    gouraudShader.bindUniformBlock("model", 0);
    gouraudShader.bindUniformBlock("camera", 1);
    gouraudShader.bindUniformBlock("light", 2);
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
    shadowShader.bindUniformBlock("model", 0);
    shadowShader.bindUniformBlock("light", 2);
  }
  graphics::buffer::UniformBuffer meshUBO, cameraUBO, lightUBO;
  // Calculate UBO alignment size
  glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignSize);
  constexpr int perMeshSize = 2 * sizeof(glm::mat4);
  constexpr int perCameraSize = sizeof(glm::mat4) + sizeof(glm::vec4);
  constexpr int perLightSize = sizeof(glm::mat4) + sizeof(glm::vec4);
  int perMeshOffset = uboAlign(perMeshSize);
  int perCameraOffset = uboAlign(perCameraSize);
  int perLightOffset = uboAlign(perLightSize);
  // We have 3 different model
  meshUBO.allocate(MESH_COUNT * perMeshOffset, GL_DYNAMIC_DRAW);
  cameraUBO.allocate(CAMERA_COUNT * perCameraOffset, GL_DYNAMIC_DRAW);
  lightUBO.allocate(LIGHT_COUNT * perLightOffset, GL_DYNAMIC_DRAW);
  // Default to first data
  meshUBO.bindUniformBlockIndex(0, 0, perMeshSize);
  cameraUBO.bindUniformBlockIndex(1, 0, perCameraSize);
  lightUBO.bindUniformBlockIndex(2, 0, perLightSize);
  // Texture
  graphics::texture::Texture2D baseColor;
  graphics::texture::ShadowMap shadow(1024);
  baseColor.fromColor(glm::vec4(1, 0.5, 0, 1));
  baseColor.bind(0);
  shadow.bind(1);
  // Setup camera.
  std::vector<graphics::camera::CameraPTR> cameras;
  cameras.emplace_back(graphics::camera::QuaternionCamera::make_unique(glm::vec3(0, 0, 15)));
  assert(cameras.size() == CAMERA_COUNT);
  for (int i = 0; i < CAMERA_COUNT; ++i) {
    int offset = i * perCameraOffset;
    cameras[i]->initialize(OpenGLContext::getAspectRatio());
    cameraUBO.load(offset, sizeof(glm::mat4), cameras[i]->getViewProjectionMatrixPTR());
    cameraUBO.load(offset + sizeof(glm::mat4), sizeof(glm::vec4), cameras[i]->getPositionPTR());
  }
  currentCamera = cameras[0].get();
  // Setup lights
  std::vector<graphics::light::LightPTR> lights;
  lights.emplace_back(graphics::light::DirectionalLight::make_unique(glm::vec3(0, 8, 6)));
  lights.emplace_back(graphics::light::PointLight::make_unique(glm::vec3(0, 8, 6)));
  assert(lights.size() == LIGHT_COUNT);
  for (int i = 0; i < LIGHT_COUNT; ++i) {
    int offset = i * perLightOffset;
    lightUBO.load(offset, sizeof(glm::mat4), lights[i]->getLightSpaceMatrixPTR());
    lightUBO.load(offset + sizeof(glm::mat4), sizeof(glm::vec4), lights[i]->getLightVectorPTR());
  }
  // Meshes
  std::vector<graphics::shape::ShapePTR> meshes;
  auto phongSphere = graphics::shape::Sphere::make_unique();
  auto gouraudSphere = graphics::shape::Sphere::make_unique();
  auto ground = graphics::shape::Plane::make_unique();
  {
    glm::mat4 model = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(1, 0, 0));
    model = glm::scale(model, glm::vec3(3));
    phongSphere->setModelMatrix(glm::translate(model, glm::vec3(-1.5, 0, 0)));
    gouraudSphere->setModelMatrix(glm::translate(model, glm::vec3(1.5, 0, 0)));

    model = glm::scale(glm::mat4(1), glm::vec3(20, 1, 20));
    model = glm::translate(model, glm::vec3(0, -3.5, 0));
    ground->setModelMatrix(model);
  }
  meshes.emplace_back(std::move(ground));
  meshes.emplace_back(std::move(gouraudSphere));
  meshes.emplace_back(std::move(phongSphere));
  assert(meshes.size() == MESH_COUNT);
  for (int i = 0; i < MESH_COUNT; ++i) {
    int offset = i * perMeshOffset;
    meshUBO.load(offset, sizeof(glm::mat4), meshes[i]->getModelMatrixPTR());
    meshUBO.load(offset + sizeof(glm::mat4), sizeof(glm::mat4), meshes[i]->getNormalMatrixPTR());
  }
  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    // Update camera's uniforms if camera moves.
    bool isCameraMove = currentCamera->move(window);
    if (isCameraMove || isWindowSizeChanged) {
      isWindowSizeChanged = false;
      cameraUBO.load(0, sizeof(glm::mat4), currentCamera->getViewProjectionMatrixPTR());
      cameraUBO.load(sizeof(glm::mat4), sizeof(glm::vec4), currentCamera->getPositionPTR());
    }
    // Switch light uniforms if light changes
    if (isLightChanged) {
      lightUBO.bindUniformBlockIndex(2, currentLight * perLightOffset, perLightSize);
      isLightChanged = false;
    }
    // Render shadow first
    glViewport(0, 0, shadow.getSize(), shadow.getSize());
    glCullFace(GL_FRONT);
    shadowShader.bind();
    shadow.bindFrameBuffer();
    glClear(GL_DEPTH_BUFFER_BIT);
    // Update model matrix
    meshUBO.bindUniformBlockIndex(0, perMeshOffset, perMeshSize);
    meshes[1]->draw();
    // Update model matrix
    meshUBO.bindUniformBlockIndex(0, 2 * perMeshOffset, perMeshSize);
    meshes[2]->draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    glViewport(0, 0, OpenGLContext::getWidth(), OpenGLContext::getHeight());
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Render gouraud shading sphere
    gouraudShader.bind();
    meshUBO.bindUniformBlockIndex(0, perMeshOffset, perMeshSize);
    meshes[1]->draw();
    // Render phong shading sphere
    phongShader.bind();
    meshUBO.bindUniformBlockIndex(0, 2 * perMeshOffset, perMeshSize);
    meshes[2]->draw();
    meshUBO.bindUniformBlockIndex(0, 0, perMeshSize);
    meshes[0]->draw();

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}
