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
    }

    int OpenRTPlotter::OpenRTPlotterInit()
    {
        int ret = CreateWindow();

        InitData();
            /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            /* Render */
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            if(Resources())
            {
                Draw();
                Input();
            }

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

        /**Draw graph**/
        glViewport(margin + ticksize, margin + ticksize, WindowWidth - margin * 2 - ticksize,   WindowHeight - margin * 2 - ticksize);

        glScissor(margin + ticksize, margin + ticksize, WindowWidth - margin * 2 - ticksize,    WindowHeight - margin * 2 - ticksize);

        glEnable(GL_SCISSOR_TEST);

        glm::mat4 Transform = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale_x, 1, 1)),     glm::vec3(offset_x, offset_y, 0));
        glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(Transform));

        GLfloat red[4] = {1, 0, 0, 1};
        glUniform4fv(uniform_color, 1, red);

        glBindBuffer(GL_ARRAY_BUFFER, Spec[0]);
        glEnableVertexAttribArray(attribute_coord2d);
        glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(1);
        glDrawArrays(GL_LINE_STRIP, 0, Specie.size());

        glBindBuffer(GL_ARRAY_BUFFER, Spec[1]);
        glEnableVertexAttribArray(attribute_coord2d);
        glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(1);
        glDrawArrays(GL_LINE_STRIP, 0, Specie2.size());

        glViewport(0, 0, WindowWidth, WindowHeight);
        glDisable(GL_SCISSOR_TEST);

        /* Draw the borders */
        float pixel_x, pixel_y;

    	// Calculate a transformation matrix that gives us the same normalized device coordinates as    above
    	Transform = ViewportTransform(margin + ticksize + 2, margin + ticksize + 2, WindowWidth -   margin * 2 - ticksize, WindowHeight - margin * 2 - ticksize, &pixel_x, &pixel_y);

    	// Tell our vertex shader about it
    	glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(Transform));

    	// Set the color to black
    	GLfloat black[4] = { 0, 0, 0, 1 };
    	glUniform4fv(uniform_color, 1, black);

    	// Draw a border around our graph
    	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(2);
    	glDrawArrays(GL_LINE_LOOP, 0, 4);

        /* ---------------------------------------------------------------- */
    	/* Draw the y tick marks */

    	point ticks[42];

    	for (int i = 0; i <= 20; i++) {
    		float y = -1 + i * 0.1;
    		float tickscale = (i % 10) ? 0.5 : 1;

    		ticks[i * 2].x = -1;
    		ticks[i * 2].y = y;
    		ticks[i * 2 + 1].x = -1 - ticksize * tickscale * pixel_x;
    		ticks[i * 2 + 1].y = y;
    	}

    	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    	glBufferData(GL_ARRAY_BUFFER, sizeof ticks, ticks, GL_DYNAMIC_DRAW);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(1);
    	glDrawArrays(GL_LINES, 0, 42);

    	/* ---------------------------------------------------------------- */
    	/* Draw the x tick marks */

    	float tickspacing = 0.1 * powf(10, -floor(log10(scale_x)));	// desired space between ticks,     in graph coordinates
    	float left = -1.0 / scale_x - offset_x;	// left edge, in graph coordinates
    	float right = 1.0 / scale_x - offset_x;	// right edge, in graph coordinates
    	int left_i = ceil(left / tickspacing);	// index of left tick, counted from the origin
    	int right_i = floor(right / tickspacing);	// index of right tick, counted from the origin
    	float rem = left_i * tickspacing - left;	// space between left edge of graph and the first   tick

    	float firsttick = -1.0 + rem * scale_x;	// first tick in device coordinates

    	int nticks = right_i - left_i + 1;	// number of ticks to show

    	if (nticks > 21)
    		nticks = 21;	// should not happen

    	for (int i = 0; i < nticks; i++) {
    		float x = firsttick + i * tickspacing * scale_x;
    		float tickscale = ((i + left_i) % 10) ? 0.5 : 1;

    		ticks[i * 2].x = x;
    		ticks[i * 2].y = -1;
    		ticks[i * 2 + 1].x = x;
    		ticks[i * 2 + 1].y = -1 - ticksize * tickscale * pixel_y;
    	}

    	glBufferData(GL_ARRAY_BUFFER, sizeof ticks, ticks, GL_DYNAMIC_DRAW);
    	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glLineWidth(1);
    	glDrawArrays(GL_LINES, 0, nticks * 2);

        glDisableVertexAttribArray(attribute_coord2d);
        //glfwSwapBuffers(window);
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

        //glBufferData(GL_ARRAY_BUFFER, sizeof graph, graph, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, Specie.size() * sizeof(point), &Specie.front(), GL_DYNAMIC_DRAW);

    	// Create a VBO for the border
    	static const point border[4] = { {-1, -1}, {1, -1}, {1, 1}, {-1, 1} };
    	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    	glBufferData(GL_ARRAY_BUFFER, sizeof border, border, GL_STATIC_DRAW);

        glGenBuffers(2, Spec);
        
        glBindBuffer(GL_ARRAY_BUFFER, Spec[0]);
        glBufferData(GL_ARRAY_BUFFER, Specie.size() * sizeof(point), &Specie.front(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, Spec[1]);
        glBufferData(GL_ARRAY_BUFFER, Specie2.size() * sizeof(point), &Specie2.front(), GL_DYNAMIC_DRAW);

        return 1;
    }

    void OpenRTPlotter::InitData()
    {
        for (int i = 0; i < 2000; i++) {
            //Sinus
    		float x = (i - 1000.0) / 100.0;

            GLfloat vx = x / 4;
            GLfloat vy = sin(x * 10.0) / (1.0 + x * x) + 1;

            point v = {vx, vy};

            Specie.push_back(v);

    		//graph[i].x = x / 4;
    		//graph[i].y = sin(x * 10.0) / (1.0 + x * x) + 1;

            //Linear
            //graph[i].x = i / 4;
            //graph[i].y = i / 4;

            vx = i / 4;
            vy = i / 4;
            v = {vx, vy};

            Specie2.push_back(v);
    	}
    }

    void OpenRTPlotter::Free()
    {
        glDeleteProgram(Program);
    }

    void OpenRTPlotter::Input()
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            if (offset_x + 0.03 <= -1)
                offset_x += 0.03;
            else
             offset_x = -1;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            offset_x -= 0.03;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            offset_y -= 0.03;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            if (offset_y + 0.03 <= -1)
                offset_y += 0.03;
            else
             offset_y = -1;
        }
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && Once == 1)
        {
            point v = {(2001 - 1000.0) / 100.0, 100};
            Specie.push_back(v);
            Once = 2;
        }
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
}