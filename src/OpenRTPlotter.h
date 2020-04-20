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

    struct Point 
    {
        GLfloat x;
        GLfloat y;
    };

    struct Plot 
    {
        std::string Name;
        glm::vec4 Color;
        std::vector<Point> Function;

        Plot(std::string TName)
        {
            Name = TName;
        }

        Plot(std::string TName, glm::vec4 TColor)
        {
            Name = TName;
            Color = TColor;
        }
    };

    class OpenRTPlotter
    {
    public:
        OpenRTPlotter();
        //OpenRTPlotter(InitStruct PlotInfo, Plot SinglePlot);
        OpenRTPlotter(InitStruct PlotInfo, std::vector<Plot> MultiPlot);

        int OpenRTPlotterInit();
        void InsertPointByName(std::string Name, Point SinglePoint);
        void InsertPointRangeByName(std::string Name, std::vector<Point> SinglePoint, int Start, int End);
        void InsertPointRangeByPlot(std::vector<Plot> Plot, int Start, int End);

    private:
        int CreateWindow();
        glm::mat4 ViewportTransform(float x, float y, float width, float height, float *pixel_x,    float *pixel_y);
        void Draw();
        void GraphDraw(glm::mat4 Transform, int WindowWidth, int WindowHeight);
        void PointDraw(Plot ToDraw);
        void BorderDraw(glm::mat4 Transform, int WindowWidth, int WindowHeight);
        void Free();
        int Resources();
        void InitData();
        void Input();
        void TestFunctions();
        InitStruct Info;

        GLFWwindow* window;

        float width, height; //Window size
        float *PixelX, *PixelY; //Pixel size
        std::vector<Plot> ToPlot; //Store all plotting data
        //TmpValues
        const int margin = 20;
        const int ticksize = 20;
        float offset_x = -10;
        float offset_y = -10;
        float scale = 0.1;
        GLuint Program;
        GLint attribute_coord2d;
        GLint uniform_color;
        GLint uniform_transform;
        GLuint vbo[3];
        GLuint Spec[2];
        GLuint PlotBuf;
        bool MultiLine;
    };
}
