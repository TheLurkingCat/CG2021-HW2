#include <algorithm>
#include <cassert>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <iostream>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
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
int currentShader = 1;
int alignSize = 256;
// Configs
constexpr int LIGHT_COUNT = 2;
constexpr int CAMERA_COUNT = 1;
constexpr int MESH_COUNT = 3;
constexpr int SHADER_PROGRAM_COUNT = 3;
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
    case GLFW_KEY_G:
      currentShader = 2;
      break;
    case GLFW_KEY_B:
      currentShader = 1;
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
  glEnable(GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex(65535);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
  // Initialize shader
  std::vector<graphics::shader::ShaderProgram> shaderPrograms(SHADER_PROGRAM_COUNT);
  std::string filenames[SHADER_PROGRAM_COUNT] = {"shadow", "phong", "gouraud"};
  for (int i = 0; i < SHADER_PROGRAM_COUNT; ++i) {
    graphics::shader::VertexShader vs;
    graphics::shader::FragmentShader fs;
    vs.fromFile("../assets/shader/" + filenames[i] + ".vert");
    fs.fromFile("../assets/shader/" + filenames[i] + ".frag");
    shaderPrograms[i].attach(&vs, &fs);
    shaderPrograms[i].link();
    shaderPrograms[i].detach(&vs, &fs);
    shaderPrograms[i].use();
    shaderPrograms[i].uniformBlockBinding("model", 0);
    shaderPrograms[i].uniformBlockBinding("camera", 1);
    shaderPrograms[i].uniformBlockBinding("light", 2);
    shaderPrograms[i].setUniform("diffuseTexture", 0);
    shaderPrograms[i].setUniform("shadowMap", 1);
    shaderPrograms[i].setUniform("diffuseCubeTexture", 2);
    shaderPrograms[i].setUniform("isCube", 0);
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
  meshUBO.allocate(MESH_COUNT * perMeshOffset, GL_DYNAMIC_DRAW);
  cameraUBO.allocate(CAMERA_COUNT * perCameraOffset, GL_DYNAMIC_DRAW);
  lightUBO.allocate(LIGHT_COUNT * perLightOffset, GL_DYNAMIC_DRAW);
  // Default to first data
  meshUBO.bindUniformBlockIndex(0, 0, perMeshSize);
  cameraUBO.bindUniformBlockIndex(1, 0, perCameraSize);
  lightUBO.bindUniformBlockIndex(2, 0, perLightSize);
  // Get texture information
  int maxTextureSize = 1024;
  // Uncomment the following line if your GPU is very poor
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  maxTextureSize = std::min(maxTextureSize, 4096);
  // Camera
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
  // Lights
  std::vector<graphics::light::LightPTR> lights;
  lights.emplace_back(graphics::light::DirectionalLight::make_unique(glm::vec3(8, 6, 6)));
  lights.emplace_back(graphics::light::PointLight::make_unique(glm::vec3(8, 6, 6)));
  assert(lights.size() == LIGHT_COUNT);
  for (int i = 0; i < LIGHT_COUNT; ++i) {
    int offset = i * perLightOffset;
    lightUBO.load(offset, sizeof(glm::mat4), lights[i]->getLightSpaceMatrixPTR());
    lightUBO.load(offset + sizeof(glm::mat4), sizeof(glm::vec4), lights[i]->getLightVectorPTR());
  }
  // Texture
  graphics::texture::ShadowMap shadow(maxTextureSize);
  graphics::texture::Texture2D colorOrange, wood;
  graphics::texture::TextureCubeMap dice;
  // This is a 1x1 single color texture
  colorOrange.fromColor(glm::vec4(1, 0.5, 0, 1));
  wood.fromFile("../assets/texture/wood.jpg");
  dice.fromFile("../assets/texture/posx.jpg",
                "../assets/texture/negx.jpg",
                "../assets/texture/posy.jpg",
                "../assets/texture/negy.jpg",
                "../assets/texture/posz.jpg",
                "../assets/texture/negz.jpg");
  // Meshes
  std::vector<graphics::shape::ShapePTR>
      meshes;
  std::vector<graphics::texture::Texture*> diffuseTextures;
  {
    std::vector<GLfloat> vertexData;
    std::vector<GLuint> indexData;
    graphics::shape::Plane::generateVertices(vertexData, indexData, 40, 20, 20, false);
    auto sphere = graphics::shape::Sphere::make_unique();
    auto cube = graphics::shape::Cube::make_unique();
    auto ground = graphics::shape::Plane::make_unique(vertexData, indexData);

    glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(3, 0, 4));
    model = glm::scale(model, glm::vec3(2));
    model = glm::rotate(model, glm::half_pi<float>(), glm::vec3(1, 0, 0));
    sphere->setModelMatrix(model);

    model = glm::translate(glm::mat4(1), glm::vec3(-3, -1, 0));
    model = glm::scale(model, glm::vec3(2));
    cube->setModelMatrix(model);

    model = glm::translate(glm::mat4(1), glm::vec3(0, -3, 0));
    ground->setModelMatrix(model);

    meshes.emplace_back(std::move(ground));
    diffuseTextures.emplace_back(&wood);
    meshes.emplace_back(std::move(sphere));
    diffuseTextures.emplace_back(&colorOrange);
    meshes.emplace_back(std::move(cube));
    diffuseTextures.emplace_back(&wood);
  }
  assert(meshes.size() == MESH_COUNT);
  assert(diffuseTextures.size() == MESH_COUNT);
  for (int i = 0; i < MESH_COUNT; ++i) {
    int offset = i * perMeshOffset;
    meshUBO.load(offset, sizeof(glm::mat4), meshes[i]->getModelMatrixPTR());
    meshUBO.load(offset + sizeof(glm::mat4), sizeof(glm::mat4), meshes[i]->getNormalMatrixPTR());
  }
  // This will not change in rendering loop
  shadow.bind(1);
  dice.bind(2);
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
    shaderPrograms[0].use();
    shadow.bindFrameBuffer();
    glClear(GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < MESH_COUNT; ++i) {
      // Update model matrix
      meshUBO.bindUniformBlockIndex(0, i * perMeshOffset, perMeshSize);
      meshes[i]->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    glViewport(0, 0, OpenGLContext::getWidth(), OpenGLContext::getHeight());
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Render all objects
    shaderPrograms[currentShader].use();
    for (int i = 0; i < MESH_COUNT; ++i) {
      if (meshes[i]->getTypeName()[0] == 'C') {
        shaderPrograms[currentShader].setUniform("isCube", 1);
      } else {
        shaderPrograms[currentShader].setUniform("isCube", 0);
      }
      meshUBO.bindUniformBlockIndex(0, i * perMeshOffset, perMeshSize);
      diffuseTextures[i]->bind(0);
      meshes[i]->draw();
    }
#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}
