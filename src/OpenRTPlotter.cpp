#include "OpenRTPlotter.h"
#include "util/DefaultShader.h"

#include <iostream>

#define MINSIZE 1000

namespace OpenRTP
{
    class Plotter::Impl
    {
        GLFWwindow* window;
        Font *mFont;
        Util *MUtil;

        InitStruct Info;
        float width, height; //Window size
        float *PixelX, *PixelY; //Pixel size
        std::vector<Plot>* ToPlot; //Store all plotting data
        //TmpValues
        const int margin = 20;
        const int ticksize = 20;
        float TickX = 1.0;
        float STickX = 0.5;

        double offset_x = -10;
        double offset_y = -10;
        double scale = 1;
        double ScaleX = 0.01;
        double ScaleY = 0.01;
        double XScale = 3;
        double YScale = 3;
        float SpeedX = 0.3;
        float SpeedY = 0.3;

        float TickspacingX = 0;
        float TickspacingY = 0;
        float Left = 0;
    	float Right = 0;
    	float Bottom = 0;
    	float Top = 0;
        
        GLuint Program;
        GLint attribute_coord2d;
        GLint uniform_color;
        GLint uniform_transform;
        GLuint vbo[3];
        GLuint Spec[2];
        GLuint LineBuf;
        GLuint PlotBuf;
        bool MultiLine;
    public:
        Impl()
        {
            MUtil = new Util();
            Info.Title = "Open Real-Time Plotter";
            Info.YName = "Y";
            Info.XName = "X";

            MultiLine = true;
        }

        Impl(InitStruct PlotInfo, std::vector<Plot>* MultiPlot)
        {
            MUtil = new Util();

            Info.Title = PlotInfo.Title;
            Info.YName = PlotInfo.XName;
            Info.XName = PlotInfo.YName;

            ToPlot = MultiPlot;
            MultiLine = true;

            if ((*ToPlot)[0].Function.size() > 0)
            {
                for (int i = 0; i < ToPlot->size(); i++)
                {
                    (*ToPlot)[i].Color = MUtil->GetUniqueColor();

                    int size = (*ToPlot)[i].Function.size() - 1;
                    auto element = (*ToPlot)[i].Function[size];
                    if(element.y > YScale)
                    {
                        YScale = ceil(element.y * 1.05);
                    }
                    if(element.x > XScale)
                    {
                        XScale = ceil(element.x * 1.05);
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

        int Init()
        {
            int ret = CreateWindow();

            InitRessources();

            mFont = new Font(window);
            int result = mFont->InitFont(MUtil);

            return ret;
        }

        int Run()
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

        void UpdatePlot()
        {
                for (int i = 0; i < ToPlot->size(); i++)
                {
                    int size = (*ToPlot)[i].Function.size() - 1;
                    auto elementEnd = (*ToPlot)[i].Function[size];
                    auto elementStart = (*ToPlot)[i].Function[0];
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

            ScaleY = 2/(YScale);
            ScaleX = 2/(XScale);

            offset_y = -(YScale/2);
            offset_x = -(XScale/2);

            SpeedX = XScale / 100;
            SpeedY = YScale / 100;
        }
    private:
        int CreateWindow()
        {
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

        glm::mat4 ViewportTransform(float x, float y, float width, float height, float *pixel_x,    float *pixel_y)
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

            return glm::scale(glm::translate(glm::mat4(1), glm::vec3(OffsetX, OffsetY, 0)), glm::vec3       (ScaleX, ScaleY, 1));
        }

        void Draw()
        {
            int WindowWidth, WindowHeight;
            glfwGetFramebufferSize(window, &WindowWidth, &WindowHeight);
            float sx = 2.0 / WindowWidth;
	        float sy = 2.0 / WindowHeight;

            glUseProgram(Program);

            glm::mat4 Transform = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(ScaleX, ScaleY, 1)   ), glm::vec3(offset_x, offset_y, 0));
            glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(Transform));

			GraphDraw(Transform, WindowWidth, WindowHeight);
            BorderDraw(Transform, WindowWidth, WindowHeight);

            glDisableVertexAttribArray(attribute_coord2d);
        }

        void GraphDraw(glm::mat4 Transform, int WindowWidth, int WindowHeight)
        {
            std::vector<Plot> Plot = *ToPlot;
            
            glViewport(margin + ticksize, margin + ticksize, WindowWidth - margin * 2 -     ticksize,       WindowHeight - margin * 2 - ticksize);

            glScissor(margin + ticksize, margin + ticksize, WindowWidth - margin * 2 -  ticksize,       WindowHeight - margin * 2 - ticksize);

            glEnable(GL_SCISSOR_TEST);
            glEnable(GL_LINE_SMOOTH);

            CalcTicks();

            GraphLine();

            for (auto func : Plot)
            {
                PointDraw(func);
            }

            glViewport(0, 0, WindowWidth, WindowHeight);

            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_SCISSOR_TEST);
        }
        
        void CalcTicks()
        {
            TickspacingX = 0.1 * powf(10, -floor(log10(ScaleX)));
            TickspacingY = 0.1 * powf(10, -floor(log10(ScaleY)));

            Left = -1.0 / ScaleX - offset_x;
    	    Right = 1.0 / ScaleX - offset_x;
    	    Bottom = -1.0 / ScaleY - offset_y;
    	    Top = 1.0 / ScaleY - offset_y;
        }

        void GraphLine()
        {
            glm::vec4 Color1 = glm::vec4(0.7, 0.7, 0.7, 1);

            Point ticks[42];

            int Left_i = ceil(Left / TickspacingX);
            int Right_i = floor(Right / TickspacingX);
            int Bottom_i = ceil(Bottom / TickspacingY);
            int Top_i = floor(Top / TickspacingY);

            float RemX = Left_i * TickspacingX - offset_x;
            float RemY = Bottom_i * TickspacingY - offset_y;

            //float FirstTickX = Left + RemX * ScaleX;
			float FirstTickX = -1 + RemX * ScaleX;
            float FirstTickY = Bottom + RemY * ScaleY;

    	    int NTicksX = Right_i - Left_i + 1;
    	    int NTicksY = Top_i - Bottom_i + 1;

            /*Draw background lines across X*/
            glUniform4fv(uniform_color, 1, glm::value_ptr(Color1));

            if (NTicksX > 21)
                NTicksX = 21;

            for (int i = 0; i < NTicksX; i++)
            {
				double x = (Left_i + i) * TickspacingX;

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
            glLineWidth(1);
    	    glDrawArrays(GL_LINES, 0, NTicksX * 2);

            /*Draw background lines across Y*/
            glUniform4fv(uniform_color, 1, glm::value_ptr(Color1));

            if (NTicksY > 21)
                NTicksY = 21;

            for (int i = 0; i < NTicksY; i++)
            {
                double y = (Bottom_i + i) * TickspacingY;

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
    	    glDrawArrays(GL_LINES, 0, NTicksY * 2);
        }
        
        void PointDraw(Plot ToDraw)
        {
            glUniform4fv(uniform_color, 1, glm::value_ptr(ToDraw.Color));

            glBindBuffer(GL_ARRAY_BUFFER, PlotBuf);
            glBufferData(GL_ARRAY_BUFFER, ToDraw.Function.size() * sizeof(Point), &ToDraw.Function.front    (), GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(attribute_coord2d);
            glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glLineWidth(3);
            glDrawArrays(GL_LINE_STRIP, 0, ToDraw.Function.size());
        }
        
        void BorderDraw(glm::mat4 Transform, int WindowWidth, int WindowHeight)
        {
            /* Draw the borders */
            float PixelX, PixelY;

            // Calculate a transformation matrix that gives us the same normalized device coordinates   as    above
    	    Transform = ViewportTransform(margin + ticksize, margin + ticksize, WindowWidth - margin * 2    - ticksize, WindowHeight - margin * 2 - ticksize, &PixelX, &PixelY);

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
            if (NTicksY > 21)
                NTicksY = 21;
    	    for (int i = 0; i < NTicksY; i++) 
            {
                float y = FirstTickY + i * TickspacingY * ScaleY;
                float TickScaleY = ((int)(i + Bottom_i) % 10) ? 0.5 : 1;
                float x = -1 - ticksize * TickScaleY * PixelX;
                float x2 = -1 - ticksize * PixelX;

                ticks[i * 2].x = -1;
    	        ticks[i * 2].y = y;
    	        ticks[i * 2 + 1].x = x;
    	        ticks[i * 2 + 1].y = y;

                glm::vec4 Pos = Transform * glm::vec4(glm::vec2(x2, y), 0, 1);

                double SInt = (Bottom_i + i) * TickspacingY;
                std::string s = std::to_string(SInt);
				s.erase(s.find_last_not_of('0') + 1, std::string::npos);
				if(s.at(s.length() - 1) == '.')
					s.erase(s.length() - 1, std::string::npos);

                mFont->TextDraw(s, NULL, Pos.x, Pos.y, RenderRight);
    	    }
            glUseProgram(Program);
            glEnableVertexAttribArray(attribute_coord2d);

    	    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    	    glBufferData(GL_ARRAY_BUFFER, sizeof(ticks), ticks, GL_DYNAMIC_DRAW);
    	    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glLineWidth(1);
    	    glDrawArrays(GL_LINES, 0, NTicksY * 2);

    	    /* Draw the x tick marks */
    	    if (NTicksX > 21)
    	    	NTicksX = 21;	// should not happen

    	    for (int i = 0; i < NTicksX; i++) {
    	    	float x = FirstTickX + i * TickspacingX * ScaleX;
    	    	float TickScaleX = ((int)(i + Left_i) % 10) ? 0.5 : 1;

                float y = -1 - ticksize * TickScaleX * PixelY;
                float y2 = -1 - ticksize * 0.5 * PixelY;

    	    	ticks[i * 2].x = x;
    	    	ticks[i * 2].y = -1;
    	    	ticks[i * 2 + 1].x = x;
    	    	ticks[i * 2 + 1].y = y;

                if (x < 1)
                {
                    glm::vec4 Pos = Transform * glm::vec4(glm::vec2(x, y2), 0, 1);
                    double SInt = (Left_i + i) * TickspacingX;
                    std::string s = std::to_string(SInt);
					s.erase(s.find_last_not_of('0') + 1, std::string::npos);
					if(s.at(s.length()-1) == '.')
						s.erase(s.length()-1, std::string::npos);

                    mFont->TextDraw(s, NULL, Pos.x, Pos.y, RenderLeft);
                }
    	    }

            glUseProgram(Program);
            glEnableVertexAttribArray(attribute_coord2d);

    	    glBufferData(GL_ARRAY_BUFFER, sizeof(ticks), ticks, GL_DYNAMIC_DRAW);
    	    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glLineWidth(1);
    	    glDrawArrays(GL_LINES, 0, NTicksX * 2);
        }
        
        int InitRessources()
        {
            Program = MUtil->CreateProgram(Vert, Frag);
    	    if (Program == 0)
            {
                std::cout << "Failed creating program\n";
    	    	return 0;
            }

            attribute_coord2d = MUtil->GetAttrib(Program, "coord2d");
    	    uniform_transform = MUtil->GetUniform(Program, "transform");
    	    uniform_color = MUtil->GetUniform(Program, "color");

            if (attribute_coord2d == -1 || uniform_transform == -1 || uniform_color == -1)
    	    	return 0;

            glGenBuffers(3, vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

            glGenBuffers(1, &PlotBuf);
            glBufferData(GL_ARRAY_BUFFER, (*ToPlot)[0].Function.size() * sizeof(Point), &(*ToPlot)[0].Function.   front(), GL_DYNAMIC_DRAW);

            glGenBuffers(1, &LineBuf);

    	    // Create a VBO for the border
    	    static const Point border[4] = { {-1, -1}, {1, -1}, {1, 1}, {-1, 1} };
    	    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    	    glBufferData(GL_ARRAY_BUFFER, sizeof border, border, GL_STATIC_DRAW);

            return 1;
        }
        
        void Input()
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                if (offset_x + SpeedX < -(XScale/2))
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
                if (offset_y + SpeedY < -(YScale/2))
                    offset_y += SpeedY;
                else
                    offset_y = -(YScale/2);
            }
            if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
            {
				ScaleAxis(false);
                if (offset_y + SpeedY > -(YScale/2))
                    offset_y = -(YScale/2);
                if (offset_x + SpeedX > -(XScale/2))
                    offset_x = -(XScale/2);
            }
            if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            {
				ScaleAxis(true);
				if (offset_y + SpeedY > -(YScale/2))
					offset_y = -(YScale/2);
				if (offset_x + SpeedX > -(XScale/2))
					offset_x = -(XScale/2);
            }
        }

		void ScaleAxis(bool In)
		{
			if(In && (ScaleX < MINSIZE && ScaleY < MINSIZE))
			{
				SpeedX /= 1.3;
				SpeedY /= 1.3;
				XScale /= 1.3;
				YScale /= 1.3;
				ScaleX = 1/(XScale/2);
				ScaleY = 1/(YScale/2);
			}			
			if (!In)
			{
				SpeedX *= 1.1;
				SpeedY *= 1.1;
				XScale *= 1.1;
				YScale *= 1.1;
				ScaleX = 1/(XScale/2);
				ScaleY = 1/(YScale/2);
			}
			else if (ScaleX > MINSIZE)
				ScaleX = MINSIZE;
			else if (ScaleY > MINSIZE)
				ScaleY = MINSIZE;
		}
        
        void Free()
        {
            mFont->Free();
            glDeleteProgram(Program);
        }
    };

    Plotter::Plotter() : PImpl{std::make_unique<Impl>()} {}

    Plotter::Plotter(InitStruct PlotInfo, std::vector<Plot>* MultiPlot): PImpl{std::make_unique<Impl>(PlotInfo, MultiPlot)} {}

    Plotter::~Plotter() = default;

    int Plotter::Init() { return PImpl->Init(); }
    int Plotter::RunPlot() { return PImpl->Run(); }
    void Plotter::UpdatePlot() { PImpl->UpdatePlot(); }
}
