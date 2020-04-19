#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace OpenRTP {
    class OpenRTPlotter;

    struct InitStruct
    {
        char *Title;            //Window title
        char *YName, *XName;    // Y & X axis names
    };

    struct point {
        GLfloat x;
        GLfloat y;
    };

    class OpenRTPlotter 
    {
    public:
        OpenRTPlotter();

        int OpenRTPlotterInit();

    private:
        int CreateWindow();
        glm::mat4 ViewportTransform(float x, float y, float width, float height, float *pixel_x,    float *pixel_y);
        void Draw();
        void Free();
        int Resources();
        void InitData();
        void Input();
        InitStruct Info;

        GLFWwindow* window;

        float width, height; //Window size
        float *PixelX, *PixelY; //Pixel size
        std::vector<point> Specie;
        std::vector<point> Specie2;
        //TmpValues
        const int margin = 20;
        const int ticksize = 20;
        float offset_x = -1;
        float offset_y = -1;
        float scale_x = 1;
        GLuint Program;
        GLint attribute_coord2d;
        GLint uniform_color;
        GLint uniform_transform;
        GLuint vbo[3];
        GLuint Spec[2];
        int Once = 1;
    };
}