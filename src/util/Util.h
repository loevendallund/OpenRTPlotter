#ifndef _Util_H
#define _Util_H
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace OpenRTP
{
    class Util
    {
    public:
        Util();
        ~Util();

        glm::vec4 GetUniqueColor();
        GLuint CreateProgram(const char* VertexString, const char* FragmentString);
        GLint GetAttrib(GLuint Program, const char* Name);
        GLint GetUniform(GLuint Program, const char* Name);
    private:
        class Impl;
        std::unique_ptr<Impl> PImpl;
    };
}
#endif