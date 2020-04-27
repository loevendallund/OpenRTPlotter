#ifndef _CREATE_SHADER_H
#define _CREATE_SHADER_H
#include <GL/glew.h>
#include <string>
char* file_read(const char* filename);
void print_log(GLuint object);
GLuint create_shader(const char *ShaderString, GLenum type);
GLuint create_shader_from_file(const char* filename, GLenum type);
GLuint create_program(const char* VertexString, const char *FragmentString);
GLuint create_program_from_file(const char* vertexfile, const char *fragmentfile);
GLuint create_gs_program(const char* vertexfile, const char *geometryfile, const char *fragmentfile, GLint input, GLint output, GLint vertices);
GLint get_attrib(GLuint program, const char *name);
GLint get_uniform(GLuint program, const char *name);
#endif