#include "Font.h"
#include <iostream>

namespace OpenRTP
{
    Font::Font(GLFWwindow *TWindow)
    {
        Window = TWindow;
    }

    int Font::Init()
    {
        /* Initialize the FreeType2 library */
    	if (FT_Init_FreeType(&ft)) {
    		fprintf(stderr, "Could not init freetype library\n");
    		return 0;
    	}

    	program = create_program("src/shader/font.v.glsl", "src/shader/font.f.glsl");
    	if(program == 0)
    		return 0;

    	attribute_coord = get_attrib(program, "coord");
    	uniform_tex = get_uniform(program, "tex");
    	uniform_color = get_uniform(program, "color");

    	if(attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
    		return 0;

    	// Create the vertex buffer object
    	glGenBuffers(1, &vbo);

    	return 1;
    }

    int Font::CreateAtlas(atlas *ToCreate, const char* FontFilename, int FontSize)
    {
        /* Load a font */
    	if (FT_New_Face(ft, FontFilename, 0, &face)) {
    		fprintf(stderr, "Could not open font %s\n", FontFilename);
    		return 0;
    	}

        //ToCreate = new atlas(face, FontSize, uniform_tex);

        return 1;
    }

    void Font::TextDraw(std::string text, atlas *FontAtlas, float mx, float my, RenderMode Mode)
    {
        int WindowWidth, WindowHeight;
        glfwGetFramebufferSize(Window, &WindowWidth, &WindowHeight);

        glUseProgram(program);
        GLfloat black[4] = { 0, 0, 0, 1 };
        glViewport(0, 0, WindowWidth, WindowHeight);

        /* Enable blending, necessary for our alpha texture */
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /* Set font size to 48 pixels, color to black */
	    glUniform4fv(uniform_color, 1, black);

        glBindTexture(GL_TEXTURE_2D, FontAtlas->tex);
        glUniform1i(uniform_tex, 0);

        glEnableVertexAttribArray(attribute_coord);
	    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	    glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
        int tc = 0;
        std::vector<TextPoint> CoordsV;

        switch (Mode)
        {
        case RenderRight:
            CoordsV = TextRight(text, &tc, FontAtlas, mx, my);
            break;
        case RenderLeft:
            CoordsV = TextLeft(text, &tc, FontAtlas, mx, my);
            break;
        }
        //std::vector<TextPoint> CoordsV = TextLeft(text, &tc, FontAtlas, mx, my);

        glViewport(0, 0, WindowWidth, WindowHeight);

        glBufferData(GL_ARRAY_BUFFER, CoordsV.size() * sizeof(TextPoint), &CoordsV.front(), GL_DYNAMIC_DRAW);
	    glDrawArrays(GL_TRIANGLES, 0, tc);

	    glDisableVertexAttribArray(attribute_coord);
    }

    std::vector<TextPoint> Font::TextLeft(std::string Text, int *NumOfChar, atlas *FontAtlas, float x, float y)
    {   
        int WindowWidth, WindowHeight;
        glfwGetFramebufferSize(Window, &WindowWidth, &WindowHeight);

        std::vector<TextPoint> Coords;

        const char *CText = Text.c_str();

        float XPos, YPos;
        float Width, Height;
        float SizeX = 2.0 / WindowWidth;
        float SizeY = 2.0 / WindowHeight;

        for (int TPointer = 0; TPointer < Text.length(); TPointer++)
        {
            *NumOfChar += 6;
            XPos = x;
            YPos = -y;
            Width = FontAtlas->c[Text[TPointer]].bw * SizeX;
            Height = FontAtlas->c[Text[TPointer]].bh * SizeY;

            x += FontAtlas->c[Text[TPointer]].ax * SizeX;
            y += FontAtlas->c[Text[TPointer]].ay * SizeY;

            if(!Width || !Height)
                continue;

            Coords.push_back((TextPoint) 
            {   XPos, 
                -YPos,
                FontAtlas->c[Text[TPointer]].tx, 
                FontAtlas->c[Text[TPointer]].ty
            });
            Coords.push_back((TextPoint) 
            {   XPos + Width, 
                -YPos, 
                FontAtlas->c[Text[TPointer]].tx + FontAtlas->c[Text[TPointer]].bw /FontAtlas->w,
                FontAtlas->c[Text[TPointer]].ty
            });
            Coords.push_back((TextPoint) 
            {   XPos, 
                -YPos - Height, 
                FontAtlas->c[Text[TPointer]].tx,
                FontAtlas->c[Text[TPointer]].ty + FontAtlas->c[Text[TPointer]].bh / FontAtlas->h
            });
            Coords.push_back((TextPoint) 
            {   XPos + Width, 
                -YPos, 
                FontAtlas->c[Text[TPointer]].tx + FontAtlas->c[Text[TPointer]].bw /FontAtlas->w,
                FontAtlas->c[Text[TPointer]].ty
            });
            Coords.push_back((TextPoint) 
            {   XPos, 
                -YPos - Height, 
                FontAtlas->c[Text[TPointer]].tx,
                FontAtlas->c[Text[TPointer]].ty + FontAtlas->c[Text[TPointer]].bh / FontAtlas->h
            });
            Coords.push_back((TextPoint) 
            {   XPos + Width,
                -YPos - Height, 
                FontAtlas->c[Text[TPointer]].tx + FontAtlas->c[Text[TPointer]].bw /FontAtlas->w,
                FontAtlas->c[Text[TPointer]].ty + FontAtlas->c[Text[TPointer]].bh / FontAtlas->h
            });
        }

        return Coords;
    }

    std::vector<TextPoint> Font::TextRight(std::string Text, int *NumOfChar, atlas *FontAtlas, float x, float y)
    {
        int WindowWidth, WindowHeight;
        glfwGetFramebufferSize(Window, &WindowWidth, &WindowHeight);

        std::vector<TextPoint> Coords;

        const char *CText = Text.c_str();

        float XPos, YPos;
        float Width, Height;
        float SizeX = 2.0 / WindowWidth;
        float SizeY = 2.0 / WindowHeight;

        for (int TPointer = Text.length() - 1; TPointer >= 0; TPointer--)
        {
            *NumOfChar += 6;
            XPos = x;
            YPos = -y;
            Width = FontAtlas->c[Text[TPointer]].bw * SizeX;
            Height = FontAtlas->c[Text[TPointer]].bh * SizeY;

            x -= FontAtlas->c[Text[TPointer]].ax * SizeX;
            y -= FontAtlas->c[Text[TPointer]].ay * SizeY;

            if(!Width || !Height)
                continue;

            Coords.push_back((TextPoint) 
            {   XPos, 
                -YPos,
                FontAtlas->c[Text[TPointer]].tx, 
                FontAtlas->c[Text[TPointer]].ty
            });
            Coords.push_back((TextPoint) 
            {   XPos + Width, 
                -YPos, 
                FontAtlas->c[Text[TPointer]].tx + FontAtlas->c[Text[TPointer]].bw /FontAtlas->w,
                FontAtlas->c[Text[TPointer]].ty
            });
            Coords.push_back((TextPoint) 
            {   XPos, 
                -YPos - Height, 
                FontAtlas->c[Text[TPointer]].tx,
                FontAtlas->c[Text[TPointer]].ty + FontAtlas->c[Text[TPointer]].bh / FontAtlas->h
            });
            Coords.push_back((TextPoint) 
            {   XPos + Width, 
                -YPos, 
                FontAtlas->c[Text[TPointer]].tx + FontAtlas->c[Text[TPointer]].bw /FontAtlas->w,
                FontAtlas->c[Text[TPointer]].ty
            });
            Coords.push_back((TextPoint) 
            {   XPos, 
                -YPos - Height, 
                FontAtlas->c[Text[TPointer]].tx,
                FontAtlas->c[Text[TPointer]].ty + FontAtlas->c[Text[TPointer]].bh / FontAtlas->h
            });
            Coords.push_back((TextPoint) 
            {   XPos + Width,
                -YPos - Height, 
                FontAtlas->c[Text[TPointer]].tx + FontAtlas->c[Text[TPointer]].bw /FontAtlas->w,
                FontAtlas->c[Text[TPointer]].ty + FontAtlas->c[Text[TPointer]].bh / FontAtlas->h
            });
        }

        return Coords;
    }
}