cmake_minimum_required(VERSION 3.10)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set the project name
project(OpenRTPlotter VERSION 0.0.3)

include_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
include_directories(src)
file(GLOB LIBSOURCES "src/*.cpp")
list(APPEND LIBSOURCES "src/util/DefaultShader.cpp" "src/util/Util.cpp")
file(GLOB LIBHEADERS "include/*.h")

find_package(glfw3 3.2 REQUIRED)

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

find_package(Freetype REQUIRED)
if (Freetype_FOUND)
  include_directories(${FREETYPE_INCLUDE_DIRS})
  link_libraries(${FREETYPE_LIBRARIES})
endif()

add_library(OpenRTP SHARED ${LIBSOURCES})
target_include_directories(OpenRTP PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${OPENGL_INCLUDE_DIRS} glfw Freetype ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(OpenRTP ${OPENGL_LIBRARIES} glfw)

install(TARGETS OpenRTP LIBRARY DESTINATION lib)
install(FILES ${LIBHEADERS} DESTINATION include/OpenRTP)

