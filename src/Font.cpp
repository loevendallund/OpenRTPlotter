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

    void Font::TextDraw(const char *text, atlas *FontAtlas, float mx, float my)
    {
        int WindowWidth, WindowHeight;
        glfwGetFramebufferSize(Window, &WindowWidth, &WindowHeight);

        const uint8_t *TPointer;
        point coords[6 * strlen(text)];
	    int c = 0;

        float BoxX = 100;
        float BoxY = 16*1.65;
        float SizeX = 2.0 / BoxX;
        float SizeY = 2.0 / BoxY;
        float x = 0;
        float y = 0;

        glUseProgram(program);
        GLfloat black[4] = { 0, 0, 0, 1 };
        glViewport(0, 0, WindowWidth, WindowHeight);

        /*Init renderbox size*/
        //float TextboxX = 200;
        //float TextboxY = 200;
        /*Init font render scaler*/
        //float SizeX = 2.0 / WindowWidth;
        //float SizeY = 2.0 / WindowHeight;

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

        for(TPointer = (const uint8_t *)text; *TPointer; TPointer++)
        {
            float XPos = x;
            float YPos = -y;
            float Width = FontAtlas->c[*TPointer].bw * SizeX;
            float Height = FontAtlas->c[*TPointer].bh * SizeY;

            x += FontAtlas->c[*TPointer].ax * SizeX;
            y += FontAtlas->c[*TPointer].ay * SizeY;

            if(!Width || !Height)
                continue;
            
            coords[c++] = (point) 
            {   XPos, 
                -YPos,
                FontAtlas->c[*TPointer].tx, 
                FontAtlas->c[*TPointer].ty
            };
            coords[c++] = (point) 
            {   XPos + Width, 
                -YPos, 
                FontAtlas->c[*TPointer].tx + FontAtlas->c[*TPointer].bw /FontAtlas->w,
                FontAtlas->c[*TPointer].ty
            };
            coords[c++] = (point) 
            {   XPos, 
                -YPos - Height, 
                FontAtlas->c[*TPointer].tx,
                FontAtlas->c[*TPointer].ty + FontAtlas->c[*TPointer].bh / FontAtlas->h
            };
            coords[c++] = (point) 
            {   XPos + Width, 
                -YPos, 
                FontAtlas->c[*TPointer].tx + FontAtlas->c[*TPointer].bw /FontAtlas->w,
                FontAtlas->c[*TPointer].ty
            };
            coords[c++] = (point) 
            {   XPos, 
                -YPos - Height, 
                FontAtlas->c[*TPointer].tx,
                FontAtlas->c[*TPointer].ty + FontAtlas->c[*TPointer].bh / FontAtlas->h
            };
            coords[c++] = (point) 
            {   XPos + Width,
                -YPos - Height, 
                FontAtlas->c[*TPointer].tx + FontAtlas->c[*TPointer].bw /FontAtlas->w,
                FontAtlas->c[*TPointer].ty + FontAtlas->c[*TPointer].bh / FontAtlas->h
            };
        }

        glViewport(mx, my, BoxX, BoxY);

        glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_DYNAMIC_DRAW);
	    glDrawArrays(GL_TRIANGLES, 0, c);

	    glDisableVertexAttribArray(attribute_coord);
        glViewport(0, 0, WindowWidth, WindowHeight);
    }
}