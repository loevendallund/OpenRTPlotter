cmake_minimum_required(VERSION 3.10)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set the project name
project(OpenPlotter VERSION 0.0.1)

include_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
include_directories(src)
file(GLOB LIBSOURCES "src/*.cpp")
message("${LIBSOURCES}")
list(REMOVE_ITEM LIBSOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

find_package(glfw3 3.3 REQUIRED)

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

add_library(OpenRTP STATIC ${LIBSOURCES})

add_executable(openplotter
	"src/main.cpp"
)

target_link_libraries(openplotter OpenRTP glfw OpenGL::GL)
