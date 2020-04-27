#include "OpenRTPlotter.h"
#include "util/ShaderUtil.h"
#include "util/DefaultShader.h"

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
        
        ToPlot = MultiPlot;
        MultiLine = true;

        if (ToPlot[0].Function.size() > 0)
        {
            for (int i = 0; i < MultiPlot.size(); i++)
            {
                int size = ToPlot[i].Function.size() - 1;
                auto element = ToPlot[i].Function[size];
                if(element.y > YScale)
                {
                    YScale = ceil(element.y);
                }
                if(element.x > XScale)
                {
                    XScale = ceil(element.x);
                }
            }
        }

        ScaleY = 2/(YScale);
        ScaleX = 2/(XScale);

        offset_y = -(YScale/2);
        offset_x = -(XScale/2);

        SpeedX = XScale / 100;
        SpeedY = YScale / 100;
    }

    int OpenRTPlotter::OpenRTPlotterInit()
    {
        int ret = CreateWindow();

        Resources();

        mFont = new Font(window);
        int result = mFont->Init();

        mFont->CreateAtlas(at, "fonts/FreeSans.ttf", 48);
        at = new Font::atlas(mFont->face, 16, mFont->uniform_tex);

        return ret;
    }
    int OpenRTPlotter::OpenRTPlotterRun()
    {
        /* Loop until the user closes the window */
        if (!glfwWindowShouldClose(window))
        {
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
        else
        {
            Free();
            glfwTerminate();
            return 1;
        }

        return 0;
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
        float sx = 2.0 / WindowWidth;
	    float sy = 2.0 / WindowHeight;

        glUseProgram(Program);

        glm::mat4 Transform = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(ScaleX, ScaleY, 1)), glm::vec3(offset_x, offset_y, 0));
        glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(Transform));
        
        GraphDraw(Transform, WindowWidth, WindowHeight);
        BorderDraw(Transform, WindowWidth, WindowHeight);

        glDisableVertexAttribArray(attribute_coord2d);
    }

    void OpenRTPlotter::GraphDraw(glm::mat4 Transform, int WindowWidth, int WindowHeight)
    {
        glViewport(margin + ticksize, margin + ticksize, WindowWidth - margin * 2 -     ticksize,   WindowHeight - margin * 2 - ticksize);

        glScissor(margin + ticksize, margin + ticksize, WindowWidth - margin * 2 -  ticksize,    WindowHeight - margin * 2 - ticksize);

        glEnable(GL_SCISSOR_TEST);
        glEnable(GL_LINE_SMOOTH);

        CalcTicks();

        GraphLine();

        for (auto func : ToPlot)
        {
            PointDraw(func);
        }

        glViewport(0, 0, WindowWidth, WindowHeight);
      
        glDisable(GL_LINE_SMOOTH);
        glEnable(GL_SCISSOR_TEST);
    }

    void OpenRTPlotter::CalcTicks()
    {
        TickspacingX = 0.1 * powf(10, -floor(log10(ScaleX)));
        TickspacingY = 0.1 * powf(10, -floor(log10(ScaleY)));

        Left = -1.0 / ScaleX - offset_x;
    	Right = 1.0 / ScaleX - offset_x;
    	Bottom = -1.0 / ScaleY - offset_y;
    	Top = 1.0 / ScaleY - offset_y;
    }

    void OpenRTPlotter::GraphLine()
    {
        glm::vec4 Color1 = glm::vec4(0, 0, 0, 1);

        Point ticks[42];

        int Left_i = ceil(Left / TickspacingX);
        int Right_i = floor(Right);
        int Bottom_i = ceil(Bottom / TickspacingY);
        int Top_i = floor(Top);

        float RemX = Left * TickspacingX - offset_x;
        float RemY = Bottom_i * TickspacingY - offset_y;

        float FirstTickX = Left + RemX * ScaleX;
        float FirstTickY = Bottom + RemY * ScaleY;

    	int NTicksX = Right_i - Left_i + 1;
    	int NTicksY = Top_i - Bottom_i + 1;

        /*Draw background lines across X*/
        glUniform4fv(uniform_color, 1, glm::value_ptr(Color1));

        if (NTicksX > 21)
            NTicksX = 21;

        for (int i = 0; i < NTicksX; i++)
        {
            //float x = FirstTickX + i * TickspacingX * ScaleX;
            float x = (Top_i + i) * TickspacingX;

    		ticks[i * 2].x = x;
    		ticks[i * 2].y = Bottom;
    		ticks[i * 2 + 1].x = x;
    		ticks[i * 2 + 1].y = Top;
    	}
        glUseProgram(Program);
        glEnableVertexAttribArray(attribute_coord2d);

        glBindBuffer(GL_ARRAY_BUFFER, LineBuf);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(ticks), ticks, GL_DYNAMIC_DRAW);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(4);
    	glDrawArrays(GL_LINES, 0, NTicksX * 2);

        /*Draw background lines across Y*/
        /*glUniform4fv(uniform_color, 1, glm::value_ptr(Color1));

        if (NTicksY > 21)
            NTicksY = 21;

        for (int i = 0; i < NTicksY; i++)
        {
            float y = (Bottom_i + i) * TickspacingY;

    		ticks[i * 2].x = Left;
    		ticks[i * 2].y = y;
    		ticks[i * 2 + 1].x = Right;
    		ticks[i * 2 + 1].y = y;
    	}
        glUseProgram(Program);
        glEnableVertexAttribArray(attribute_coord2d);

        glBindBuffer(GL_ARRAY_BUFFER, LineBuf);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(ticks), ticks, GL_DYNAMIC_DRAW);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(1);
    	glDrawArrays(GL_LINES, 0, NTicksY * 2);*/
        
    }

    void OpenRTPlotter::PointDraw(Plot ToDraw)
    {
        glUniform4fv(uniform_color, 1, glm::value_ptr(ToDraw.Color));

        glBindBuffer(GL_ARRAY_BUFFER, PlotBuf);
        glBufferData(GL_ARRAY_BUFFER, ToDraw.Function.size() * sizeof(Point), &ToDraw.Function.front(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(attribute_coord2d);
        glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(3);
        glDrawArrays(GL_LINE_STRIP, 0, ToDraw.Function.size());
    }

    void OpenRTPlotter::BorderDraw(glm::mat4 Transform, int WindowWidth, int WindowHeight)
    {
        /* Draw the borders */
        float PixelX, PixelY;

        // Calculate a transformation matrix that gives us the same normalized device coordinates as    above
    	Transform = ViewportTransform(margin + ticksize, margin + ticksize, WindowWidth - margin * 2 - ticksize, WindowHeight - margin * 2 - ticksize, &PixelX, &PixelY);

        // Tell our vertex shader about it
    	glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(Transform));

        // Set the color to black
    	GLfloat black[4] = { 0, 0, 0, 1 };
    	glUniform4fv(uniform_color, 1, black);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(2);
    	glDrawArrays(GL_LINE_LOOP, 0, 4);

        Point ticks[42];

        int Left_i = ceil(Left / TickspacingX);
        int Right_i = floor(Right / TickspacingX);
        int Bottom_i = ceil(Bottom / TickspacingY);
        int Top_i = floor(Top / TickspacingY);

        float RemX = Left_i * TickspacingX - Left;
        float RemY = Bottom_i * TickspacingY - Bottom;

        float FirstTickX = -1 + RemX * ScaleX;
        float FirstTickY = -1 + RemY * ScaleY;

    	int NTicksX = Right_i - Left_i + 1;
    	int NTicksY = Top_i - Bottom_i + 1;

    	/* Draw the y tick marks */
        int SizeY_I = floor(YScale / 2);

        if (NTicksY > 21)
            NTicksY = 21;
    	for (int i = 0; i < NTicksY; i++) 
        {
            float y = FirstTickY + i * TickspacingY * ScaleY;
            float TickScaleY = ((int)(i + Bottom_i) % SizeY_I) ? 0.5 : 1;

            float x = -1 - ticksize * TickScaleY * PixelX;
            float x2 = -1 - ticksize * PixelX;

            ticks[i * 2].x = -1;
    	    ticks[i * 2].y = y;
    	    ticks[i * 2 + 1].x = x;
    	    ticks[i * 2 + 1].y = y;

            glm::vec4 Pos = Transform * glm::vec4(glm::vec2(x2, y), 0, 1);

            int SInt = (Bottom_i + i) * TickspacingY;
            std::string s = std::to_string(SInt);

            mFont->TextDraw(s, at, Pos.x, Pos.y, RenderRight);
    	}
        glUseProgram(Program);
        glEnableVertexAttribArray(attribute_coord2d);

    	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(ticks), ticks, GL_DYNAMIC_DRAW);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(1);
    	glDrawArrays(GL_LINES, 0, NTicksY * 2);

    	/* Draw the x tick marks */
        int SizeX_I = floor(XScale / 2);

    	if (NTicksX > 21)
    		NTicksX = 21;	// should not happen

    	for (int i = 0; i < NTicksX; i++) {
    		float x = FirstTickX + i * TickspacingX * ScaleX;
    		float TickScaleX = ((int)(i + Left_i) % SizeX_I) ? 0.5 : 1;

            float y = -1 - ticksize * TickScaleX * PixelY;
            float y2 = -1 - ticksize * 0.5 * PixelY;

    		ticks[i * 2].x = x;
    		ticks[i * 2].y = -1;
    		ticks[i * 2 + 1].x = x;
    		ticks[i * 2 + 1].y = y;

            if (x < 1)
            {
                glm::vec4 Pos = Transform * glm::vec4(glm::vec2(x, y2), 0, 1);
                int SInt = (Left_i + i) * TickspacingX;
                std::string s = std::to_string(SInt);
                
                mFont->TextDraw(s, at, Pos.x, Pos.y, RenderLeft);
            }
    	}

        glUseProgram(Program);
        glEnableVertexAttribArray(attribute_coord2d);

    	glBufferData(GL_ARRAY_BUFFER, sizeof(ticks), ticks, GL_DYNAMIC_DRAW);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(1);
    	glDrawArrays(GL_LINES, 0, NTicksX * 2);
    }

    int OpenRTPlotter::Resources()
    {
        Program = create_program(Vert, Frag);
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

        glGenBuffers(1, &PlotBuf);
        glBufferData(GL_ARRAY_BUFFER, ToPlot[0].Function.size() * sizeof(Point), &ToPlot[0].Function.front(), GL_DYNAMIC_DRAW);
        
        glGenBuffers(1, &LineBuf);

    	// Create a VBO for the border
    	static const Point border[4] = { {-1, -1}, {1, -1}, {1, 1}, {-1, 1} };
    	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    	glBufferData(GL_ARRAY_BUFFER, sizeof border, border, GL_STATIC_DRAW);

        return 1;
    }

    void OpenRTPlotter::Free()
    {
        mFont->Free();
        glDeleteProgram(Program);
    }

    void OpenRTPlotter::Input()
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            if (offset_x + SpeedX <= -(XScale/2))
                offset_x += SpeedX;
            else
                offset_x = -(XScale/2);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            offset_x -= SpeedX;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            offset_y -= SpeedY;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            if (offset_y + SpeedY <= -(YScale/2))
                offset_y += SpeedY;
            else
                offset_y = -(YScale/2);
        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        {
            SpeedX *= 1.1;
            SpeedY *= 1.1;
            YScale *= 1.1;
            XScale *= 1.1;
            ScaleY = 1/(YScale/2);
            ScaleX = 1/(XScale/2);
            if (offset_y + 0.3 > -(YScale/2))
                offset_y = -(YScale/2);
            if (offset_x + 0.3 > -(XScale/2))
                offset_x = -(XScale/2);
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            SpeedX /= 1.3;
            SpeedY /= 1.3;
            YScale /= 1.3;
            XScale /= 1.3;
            ScaleY = 1/(YScale/2);
            ScaleX = 1/(XScale/2);
            if (offset_y + 0.3 > -(YScale/2))
                offset_y = -(YScale/2);
            if (offset_x + 0.3 > -(XScale/2))
                offset_x = -(XScale/2);
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
        const char *Title = Info.Title.c_str();
        window = glfwCreateWindow(640, 480, Title, NULL, NULL);
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
    
    void OpenRTPlotter::InsertByPlot(std::vector<Plot> Plot)
    {
        ToPlot = Plot;

        if (ToPlot[0].Function.size() > 0)
        {
            for (int i = 0; i < ToPlot.size(); i++)
            {
                int size = ToPlot[i].Function.size() - 1;
                auto elementEnd = ToPlot[i].Function[size];
                auto elementStart = ToPlot[i].Function[0];
                if(elementEnd.y > YScale)
                {
                    YScale = ceil(elementEnd.y * 1.05);
                }
                if(elementEnd.x > XScale)
                {
                    XScale = ceil(elementEnd.x * 1.05);
                }
                if(elementStart.y > YScale)
                {
                    YScale = ceil(elementStart.y * 1.05);
                }
                if(elementStart.x > XScale)
                {
                    XScale = ceil(elementStart.x * 1.05);
                }
            }
        }

        ScaleY = 2/(YScale);
        ScaleX = 2/(XScale);

        offset_y = -(YScale/2);
        offset_x = -(XScale/2);

        SpeedX = XScale / 100;
        SpeedY = YScale / 100;
    }
}