project(HW2 C CXX)

set(HW2_SOURCE
  ${HW2_SOURCE_DIR}/buffer/vertexarray.cpp
  ${HW2_SOURCE_DIR}/camera/camera.cpp
  ${HW2_SOURCE_DIR}/camera/quat_camera.cpp
  ${HW2_SOURCE_DIR}/context_manager.cpp
  ${HW2_SOURCE_DIR}/shader/shader.cpp
  ${HW2_SOURCE_DIR}/shape/sphere.cpp
  ${HW2_SOURCE_DIR}/shader/program.cpp
  ${HW2_SOURCE_DIR}/main.cpp
)

set(HW2_INCLUDE_DIR ${HW2_SOURCE_DIR}/../include)

set(HW2_HEADER
  ${HW2_INCLUDE_DIR}/filesystem.h
  ${HW2_INCLUDE_DIR}/buffer/buffer.h
  ${HW2_INCLUDE_DIR}/buffer/vertexarray.h
  ${HW2_INCLUDE_DIR}/camera/camera.h
  ${HW2_INCLUDE_DIR}/camera/quat_camera.h
  ${HW2_INCLUDE_DIR}/shape/shape.h
  ${HW2_INCLUDE_DIR}/shape/sphere.h
  ${HW2_INCLUDE_DIR}/shader/shader.h
  ${HW2_INCLUDE_DIR}/shader/program.h
  ${HW2_INCLUDE_DIR}/context_manager.h
  ${HW2_INCLUDE_DIR}/utils.h

)
add_executable(HW2 ${HW2_SOURCE} ${HW2_HEADER})
target_include_directories(HW2 PRIVATE ${HW2_INCLUDE_DIR})

add_dependencies(HW2 glad glfw glm)
# Can include glfw and glad in arbitrary order
target_compile_definitions(HW2 PRIVATE GLFW_INCLUDE_NONE)
# More warnings
if (NOT MSVC)
  target_compile_options(HW2
    PRIVATE "-Wall"
    PRIVATE "-Wextra"
    PRIVATE "-Wpedantic"
  )
endif()
# Prefer std c++20, at least need c++17 to compile
set_target_properties(HW2 PROPERTIES
  CXX_STANDARD 20
  CXX_EXTENSIONS OFF
)

target_link_libraries(HW2
  PRIVATE glad
  PRIVATE glfw
)

if (TARGET glm::glm_shared)
  target_link_libraries(HW2 PRIVATE glm::glm_shared)
elseif(TARGET glm::glm_static)
  target_link_libraries(HW2 PRIVATE glm::glm_static)
else()
  target_link_libraries(HW2 PRIVATE glm::glm)
endif()
