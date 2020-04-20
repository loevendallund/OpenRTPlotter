#include "OpenRTPlotter.h"
#include "ShaderUtil.h"

#include <iostream>

namespace OpenRTP
{
    OpenRTPlotter::OpenRTPlotter()
    {
        Info.Title = "Open Real-Time Plotter";
        Info.YName = "Y";
        Info.XName = "X";

        TestFunctions();
        MultiLine = true;
    }

    /*OpenRTPlotter::OpenRTPlotter(InitStruct PlotInfo, Plot SinglePlot)
    {
        Info.Title = PlotInfo.Title;
        Info.YName = PlotInfo.XName;
        Info.XName = PlotInfo.YName;

        ToPlot.push_back(SinglePlot);
        MultiLine = false;
    }*/

    OpenRTPlotter::OpenRTPlotter(InitStruct PlotInfo, std::vector<Plot> MultiPlot)
    {
        Info.Title = PlotInfo.Title;
        Info.YName = PlotInfo.XName;
        Info.XName = PlotInfo.YName;

        /*for (int i = 0; i < MultiPlot.size(); i++)
        {
            ToPlot.push_back(MultiPlot[i].Name);
            ToPlot[i].Color = MultiPlot[i].Color;

            for (int j = 0; j < MultiPlot[i].Function.size(); j++)
            {
                ToPlot[i].Function.push_back(MultiPlot[i].Function[j]);
            }
            
        }*/
        
        ToPlot = MultiPlot;
        //TestFunctions();
        MultiLine = true;
    }

    int OpenRTPlotter::OpenRTPlotterInit()
    {
        int ret = CreateWindow();

        Resources();

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            /* Render */
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            Draw();
            Input();

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        Free();
        glfwTerminate();
        return ret;
    }

    glm::mat4 OpenRTPlotter::ViewportTransform(float x, float y, float width, float height, float   *pixel_x = 0, float *pixel_y = 0)
    {
        int WindowWidth, WindowHeight;
        glfwGetFramebufferSize(window, &WindowWidth, &WindowHeight);

        float OffsetX = (2 * x + (width - WindowWidth)) / WindowWidth;
        float OffsetY = (2 * y + (height - WindowHeight)) / WindowHeight;

        float ScaleX = width / WindowWidth;
        float ScaleY = height / WindowHeight;

        if (pixel_x)
    		*pixel_x = 2.0 / width;
    	if (pixel_y)
    		*pixel_y = 2.0 / height;

        return glm::scale(glm::translate(glm::mat4(1), glm::vec3(OffsetX, OffsetY, 0)), glm::vec3   (ScaleX, ScaleY, 1));
    }

    void OpenRTPlotter::Draw()
    {
        int WindowWidth, WindowHeight;
        glfwGetFramebufferSize(window, &WindowWidth, &WindowHeight);

        glUseProgram(Program);

        glm::mat4 Transform = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1)), glm::vec3(offset_x, offset_y, 0));
        glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(Transform));

        /**Draw graph**/
        
        GraphDraw(Transform, WindowWidth, WindowHeight);

        BorderDraw(Transform, WindowWidth, WindowHeight);

        glDisableVertexAttribArray(attribute_coord2d);
    }

    void OpenRTPlotter::GraphDraw(glm::mat4 Transform, int WindowWidth, int WindowHeight)
    {
        glViewport(margin + ticksize, margin + ticksize, WindowWidth - margin * 2 -     ticksize,   WindowHeight - margin * 2 - ticksize);

        glScissor(margin + ticksize, margin + ticksize, WindowWidth - margin * 2 -  ticksize,    WindowHeight - margin * 2 - ticksize);

        glEnable(GL_SCISSOR_TEST);

        for (auto func : ToPlot)
        {
            PointDraw(func);
        }

        glViewport(0, 0, WindowWidth, WindowHeight);
        glDisable(GL_SCISSOR_TEST);
    }

    void OpenRTPlotter::PointDraw(Plot ToDraw)
    {
        glUniform4fv(uniform_color, 1, glm::value_ptr(ToDraw.Color));

        glBindBuffer(GL_ARRAY_BUFFER, PlotBuf);
        glBufferData(GL_ARRAY_BUFFER, ToDraw.Function.size() * sizeof(Point), &ToDraw.Function.front(), GL_DYNAMIC_DRAW);

        //glBindBuffer(GL_ARRAY_BUFFER, ToDraw.Function[0]);
        glEnableVertexAttribArray(attribute_coord2d);
        glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(1);
        glDrawArrays(GL_LINE_STRIP, 0, ToDraw.Function.size());
    }

    void OpenRTPlotter::BorderDraw(glm::mat4 Transform, int WindowWidth, int WindowHeight)
    {
        /* Draw the borders */
        float PixelX, PixelY;

        // Calculate a transformation matrix that gives us the same normalized device coordinates as    above
    	Transform = ViewportTransform(margin + ticksize + 2, margin + ticksize + 2, WindowWidth -   margin * 2 - ticksize, WindowHeight - margin * 2 - ticksize, &PixelX, &PixelY);

        // Tell our vertex shader about it
    	glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(Transform));

        // Set the color to black
    	GLfloat black[4] = { 0, 0, 0, 1 };
    	glUniform4fv(uniform_color, 1, black);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(2);
    	glDrawArrays(GL_LINE_LOOP, 0, 4);

        float tickspacing = 0.1 * powf(10, -floor(log10(scale)));	// desired space between ticks, in graph coordinates

    	/* Draw the y tick marks */
        float bottom = -1 / scale - offset_y;
        float top = 1 / scale - offset_y;
        int bottom_i = ceil(bottom / tickspacing);
        int top_i = floor(top / tickspacing);
        float remy = bottom_i * tickspacing - bottom;
        float firstticky = -1 + remy * scale;
        int nticksy = top_i - bottom_i + 1;

    	Point ticks[42];

        if (nticksy > 21)
            nticksy = 21;
    	for (int i = 0; i < nticksy; i++) {
            float y = firstticky + i * tickspacing * scale;
            float tickscaley = ((i + bottom_i) % 10) ? 0.5 : 1;

            ticks[i * 2].x = -1;
    		ticks[i * 2].y = y;
    		ticks[i * 2 + 1].x = -1 - ticksize * tickscaley * PixelX;
    		ticks[i * 2 + 1].y = y;
    	}

    	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    	glBufferData(GL_ARRAY_BUFFER, sizeof ticks, ticks, GL_DYNAMIC_DRAW);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(1);
    	glDrawArrays(GL_LINES, 0, 42);

    	/* Draw the x tick marks */
    	float left = -1.0 / scale - offset_x;	// left edge, in graph coordinates
    	float right = 1.0 / scale - offset_x;	// right edge, in graph coordinates
    	int left_i = ceil(left / tickspacing);	// index of left tick, counted from the origin
    	int right_i = floor(right / tickspacing);	// index of right tick, counted from the origin
    	float rem = left_i * tickspacing - left;	// space between left edge of graph and the first   tick

    	float firsttick = -1.0 + rem * scale;	// first tick in device coordinates

    	int nticks = right_i - left_i + 1;	// number of ticks to show

    	if (nticks > 21)
    		nticks = 21;	// should not happen

    	for (int i = 0; i < nticks; i++) {
    		float x = firsttick + i * tickspacing * scale;
    		float tickscale = ((i + left_i) % 10) ? 0.5 : 1;

    		ticks[i * 2].x = x;
    		ticks[i * 2].y = -1;
    		ticks[i * 2 + 1].x = x;
    		ticks[i * 2 + 1].y = -1 - ticksize * tickscale * PixelY;
    	}

    	glBufferData(GL_ARRAY_BUFFER, sizeof ticks, ticks, GL_DYNAMIC_DRAW);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(1);
    	glDrawArrays(GL_LINES, 0, nticks * 2);
    }

    int OpenRTPlotter::Resources()
    {
        Program = create_program("src/shader/v.glsl", "src/shader/f.glsl");
    	if (Program == 0)
        {
            std::cout << "Failed creating program\n";
    		return 0;
        }

        attribute_coord2d = get_attrib(Program, "coord2d");
    	uniform_transform = get_uniform(Program, "transform");
    	uniform_color = get_uniform(Program, "color");

        if (attribute_coord2d == -1 || uniform_transform == -1 || uniform_color == -1)
    		return 0;

        glGenBuffers(3, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

        glBufferData(GL_ARRAY_BUFFER, ToPlot[0].Function.size() * sizeof(Point), &ToPlot[0].Function.front(), GL_DYNAMIC_DRAW);

    	// Create a VBO for the border
    	static const Point border[4] = { {-1, -1}, {1, -1}, {1, 1}, {-1, 1} };
    	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    	glBufferData(GL_ARRAY_BUFFER, sizeof border, border, GL_STATIC_DRAW);

        // Generates buffer for the graph lines
        glGenBuffers(1, &PlotBuf);

        return 1;
    }

    void OpenRTPlotter::Free()
    {
        glDeleteProgram(Program);
    }

    void OpenRTPlotter::Input()
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            if (offset_x + 0.3 <= -10)
                offset_x += 0.3;
            else
             offset_x = -10;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            offset_x -= 0.3;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            offset_y -= 0.3;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            if (offset_y + 0.3 <= -10)
                offset_y += 0.3;
            else
             offset_y = -10;
        }
    }

    //Test function, to init test functions and display them, will be set if OpemRTPlotter constructor is empty
    void OpenRTPlotter::TestFunctions()
    {
        ToPlot.push_back({"x1"});
        ToPlot.push_back({"x2"});

        //std::vector<Point> x1, x2;
        for (int i = 0; i < 2000; i++) {
            //Sinus
    		float x = (i - 1000.0) / 100.0;

            GLfloat vx = i;
            GLfloat vy = sin(i * 10.0) / (1.0 + i * i) + 1;

            Point v = {vx, vy};

            ToPlot[0].Function.push_back(v);

            vx = x / 4;
            vy = x / 4;
            v = {vx, vy};

            ToPlot[1].Function.push_back(v);
    	}

        ToPlot[0].Color = glm::vec4(0, 0, 1, 1);
        ToPlot[0].Color = glm::vec4(1, 0, 0, 1);
    }

    int OpenRTPlotter::CreateWindow()
    {
        //glfwSetErrorCallback(error_callback);

        /* Initialize the library */
        if (!glfwInit())
        {
            exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(640, 480, Info.Title, NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }


        glfwSwapInterval(1);

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        glewExperimental = GL_TRUE;
        glewInit();

        return 0;
    }

    void OpenRTPlotter::InsertPointByName(std::string Name, Point SinglePoint)
    {
        ToPlot[0].Function.push_back(SinglePoint);
        std::cout << SinglePoint.y << std::endl;
    }

    void OpenRTPlotter::InsertPointRangeByName(std::string Name, std::vector<Point> SinglePoint, int Start, int End)
    {
        for (int i = Start; i < End; i++)
        {
            ToPlot[0].Function.push_back(SinglePoint[i]);
        }
    }
    
    void OpenRTPlotter::InsertPointRangeByPlot(std::vector<Plot> Plot, int Start, int End)
    {
        for (int i = 0; i < Plot.size(); i++)
        {
            for (int j = Start; j < End; j++)
            {
                ToPlot[i].Function.push_back(Plot[i].Function[j]);
            }
        }
    }
}