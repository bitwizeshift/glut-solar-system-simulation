cmake_minimum_required(VERSION 3.10)

project(SolarSystem LANGUAGES CXX)

set(source_files "src/main.cpp")

add_executable(${PROJECT_NAME}
  ${source_files}
)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

target_compile_features(${PROJECT_NAME}
  PRIVATE cxx_std_98
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE GLUT::GLUT OpenGL::OpenGL OpenGL::GLU
)