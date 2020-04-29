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

#include "Font.h"
#include "../src/util/Util.h"

namespace OpenRTP {
    struct InitStruct
    {
        std::string Title;            //Window title
        std::string YName, XName;    // Y & X axis names
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

    class Plotter
    {
    public:
        Plotter();
        Plotter(InitStruct PlotInfo, std::vector<Plot>* MultiPlot);
        ~Plotter();

        int Init();
        int RunPlot();
        void UpdatePlot();

    private:
        class Impl;
        std::unique_ptr<Impl> PImpl;
    };
}
