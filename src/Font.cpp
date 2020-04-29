#include "Font.h"

#include "util/DefaultShader.h"
#include <iostream>

namespace OpenRTP
{
    class Font::Impl
    {
        GLFWwindow* Window;
  	    GLuint Program;
  	    GLint AttribCoord;
  	    GLint UniformColor;
  	    GLuint vbo;

  	    FT_Library ft;
    public:
        Impl(GLFWwindow* TWindow)
        {
            Window = TWindow;
        }

        int InitFont(Util* MUtil, GLint* UniformTex)
        {
            /* Initialize the FreeType2 library */
    	    if (FT_Init_FreeType(&ft)) {
    	    	fprintf(stderr, "Could not init freetype library\n");
    	    	return 0;
    	    }

    	    Program = MUtil->CreateProgram(FontVert, FontFrag);
    	    if(Program == 0)
    	    	return 0;

    	    AttribCoord = MUtil->GetAttrib(Program, "coord");
    	    *UniformTex = MUtil->GetUniform(Program, "tex");
    	    UniformColor = MUtil->GetUniform(Program, "color");

    	    if(AttribCoord == -1 || *UniformTex == -1 || UniformColor == -1)
    	    	return 0;

    	    // Create the vertex buffer object
    	    glGenBuffers(1, &vbo);

    	    return 1;
        }

        int CreateAtlas(atlas *ToCreate, const char* FontFilename, int FontSize, FT_Face *FontFace)
        {
            /* Load a font */
    	    if (FT_New_Face(ft, FontFilename, 0, FontFace)) {
    	    	fprintf(stderr, "Could not open font %s\n", FontFilename);
    	    	return 0;
    	    }

            //ToCreate = new atlas(face, FontSize, uniform_tex);

            return 1;
        }

        void TextDraw(std::string text, atlas *FontAtlas, float mx, float my, RenderMode Mode, GLint UniformTex)
        {
            int WindowWidth, WindowHeight;
            glfwGetFramebufferSize(Window, &WindowWidth, &WindowHeight);

            glUseProgram(Program);
            GLfloat black[4] = { 0, 0, 0, 1 };
            glViewport(0, 0, WindowWidth, WindowHeight);

            /* Enable blending, necessary for our alpha texture */
	        glEnable(GL_BLEND);
	        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            /* Set font size to 48 pixels, color to black */
	        glUniform4fv(UniformColor, 1, black);

            glBindTexture(GL_TEXTURE_2D, FontAtlas->tex);
            glUniform1i(UniformTex, 0);

            glEnableVertexAttribArray(AttribCoord);
	        glBindBuffer(GL_ARRAY_BUFFER, vbo);
	        glVertexAttribPointer(AttribCoord, 4, GL_FLOAT, GL_FALSE, 0, 0);
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

            glBufferData(GL_ARRAY_BUFFER, CoordsV.size() * sizeof(TextPoint), &CoordsV.front(),     GL_DYNAMIC_DRAW);
	        glDrawArrays(GL_TRIANGLES, 0, tc);

	        glDisableVertexAttribArray(AttribCoord);
        }

        void Free()
        {
            glDeleteProgram(Program);
        }

    private:
        std::vector<TextPoint> TextLeft(std::string Text, int *NumOfChar, atlas *FontAtlas, float x, float y)
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

        std::vector<TextPoint> TextRight(std::string Text, int *NumOfChar, atlas *FontAtlas, float x, float y)
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
    };
    
    Font::Font(GLFWwindow* TWindow) : PImpl{std::make_unique<Impl>(TWindow)} {}
    Font::~Font() = default;

    int Font::InitFont(Util* MUtil) { return PImpl->InitFont(MUtil, &UniformTex); }
    int Font::CreateAtlas(atlas *ToCreate, const char* FontFilename, int FontSize) { return PImpl->CreateAtlas(ToCreate, FontFilename, FontSize, &Face); }
    void Font::TextDraw(std::string text, atlas *FontAtlas, float mx, float my, RenderMode Mode) { PImpl->TextDraw(text, FontAtlas, mx, my, Mode, UniformTex); }
	void Font::Free() { PImpl->Free(); }
}