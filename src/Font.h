#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>

#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderUtil.h"
#define MAXWIDTH 1024

namespace OpenRTP
{
	enum RenderMode {RenderRight, RenderLeft};

	struct TextPoint {
		GLfloat x;
		GLfloat y;
		GLfloat s;
		GLfloat t;
	};

  	class Font
  	{
  	public:
  	  struct atlas;
  	  Font(GLFWwindow* TWindow);

  	  int Init();
  	  int CreateAtlas(atlas *ToCreate, const char* FontFilename, int FontSize);
  	  void TextDraw(std::string text, atlas *FontAtlas, float mx, float my, RenderMode Mode);

  	  FT_Face face;
  	  GLint uniform_tex;
  	private:
		std::vector<TextPoint> TextLeft(std::string Text, int *NumOfChar, atlas *FontAtlas, float 	x, float y);
		std::vector<TextPoint> TextRight(std::string Text, int *NumOfChar, atlas *FontAtlas, 	float x, float y);

  	  GLFWwindow* Window;
  	  GLuint program;
  	  GLint attribute_coord;
  	  GLint uniform_color;
  	  GLuint vbo;

  	  FT_Library ft;

	public:
    struct atlas {
  	GLuint tex;		// texture object

  	unsigned int w;			// width of texture in pixels
  	unsigned int h;			// height of texture in pixels

  	struct {
  		float ax;	// advance.x
  		float ay;	// advance.y

  		float bw;	// bitmap.width;
  		float bh;	// bitmap.height;

  		float bl;	// bitmap_left;
  		float bt;	// bitmap_top;

  		float tx;	// x offset of glyph in texture coordinates
  		float ty;	// y offset of glyph in texture coordinates
  	} c[128];		// character information

  	 atlas(FT_Face face, int height, GLint uniform_tex) {
  		FT_Set_Pixel_Sizes(face, 0, height);
  		FT_GlyphSlot g = face->glyph;

  		unsigned int roww = 0;
		unsigned int rowh = 0;
  		 w = 0;
  		 h = 0;

  		 memset(c, 0, sizeof c);

  		/* Find minimum size for a texture holding all visible ASCII characters */
  		for (int i = 32; i < 128; i++) {
  			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
  				fprintf(stderr, "Loading character %c failed!\n", i);
  				continue;
  			}
  			if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
  				w = std::max(w, roww);
  				h += rowh;
  				roww = 0;
  				rowh = 0;
  			}
  			roww += g->bitmap.width + 1;
  			rowh = std::max(rowh, g->bitmap.rows);
  		}

  		w = std::max(w, roww);
  		h += rowh;

  		/* Create a texture that will be used to hold all ASCII glyphs */
  		glActiveTexture(GL_TEXTURE0);
  		glGenTextures(1, &tex);
  		glBindTexture(GL_TEXTURE_2D, tex);
  		glUniform1i(uniform_tex, 0);

  		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

  		/* We require 1 byte alignment when uploading texture data */
  		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  		/* Clamping to edges is important to prevent artifacts when scaling */
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  		/* Linear filtering usually looks best for text */
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  		/* Paste all glyph bitmaps into the texture, remembering the offset */
  		int ox = 0;
  		int oy = 0;

  		rowh = 0;

  		for (int i = 32; i < 128; i++) {
  			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
  				fprintf(stderr, "Loading character %c failed!\n", i);
  				continue;
  			}

  			if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
  				oy += rowh;
  				rowh = 0;
  				ox = 0;
  			}

  			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA,  GL_UNSIGNED_BYTE, g->bitmap.buffer);
  			c[i].ax = g->advance.x >> 6;
  			c[i].ay = g->advance.y >> 6;

  			c[i].bw = g->bitmap.width;
  			c[i].bh = g->bitmap.rows;

  			c[i].bl = g->bitmap_left;
  			c[i].bt = g->bitmap_top;

  			c[i].tx = ox / (float)w;
  			c[i].ty = oy / (float)h;

  			rowh = std::max(rowh, g->bitmap.rows);
  			ox += g->bitmap.width + 1;
  		}

  		fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", w, h, w * h / 1024);
  	  }

  	  ~atlas() {
  	  	glDeleteTextures(1, &tex);
  	  }
    };
  };
}