project(nano_particles)
cmake_minimum_required(VERSION 2.8)

#message( ${GLAD_SOURCES} )

set(GLAD "$ENV{GLAD}")
set(ASSIMP "$ENV{ASSIMP}")
set(PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")
set(UTILS_SOURCES "../CORE_2_0/Utils/*.cpp")
set(UTILS_INCLUDE_DIRS "../CORE_2_0/Utils")
set(PROJECT_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/")
set(GLAD_SOURCES ${GLAD}/src/glad.c)
set(GLAD_INCLUDE_DIRS ${GLAD}/include)
set(ASSIMP_INCLUDE_DIRS ${ASSIMP}/include)


find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
set (OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL  REQUIRED)
find_package(GLUT REQUIRED)

include_directories(
  ${GLFW_INCLUDE_DIRS}
  ${GLAD_INCLUDE_DIRS}
  ${UTILS_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
  ${ASSIMP_INCLUDE_DIRS}
  )

file(GLOB SOURCES ${PROJECT_SOURCES} ${GLAD_SOURCES} ${UTILS_SOURCES})


if(NOT MSVC)
  find_library(M_LIB m)
else() # not needed on Windows
  set(M_LIB "")
endif()

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(
  ${PROJECT_NAME}
  ${GLFW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${M_LIB}
  ${CMAKE_DL_LIBS}
  ${GLUT_LIBRARY}
  )
