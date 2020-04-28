#ifndef _CREATE_SHADER_H
#define _CREATE_SHADER_H
#include <GL/glew.h>
#include <string>

void print_log(GLuint object);
GLuint create_shader(const char *ShaderString, GLenum type);
GLuint create_program(const char* VertexString, const char *FragmentString);
GLint get_attrib(GLuint program, const char *name);
GLint get_uniform(GLuint program, const char *name);
#endif