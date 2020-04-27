#include "DefaultShader.h"

const char *Vert = "attribute vec2 coord2d;"
    "uniform mat4 transform;" 
    "void main(void) {"
        "gl_Position = transform * vec4(coord2d.xy, 0, 1);" 
    "}";

const char *Frag = "uniform vec4 color;"
    "void main(void) {"
        "gl_FragColor = color;"
    "}";

const char *FontVert = "attribute vec4 coord;"
    "varying vec2 texpos;"
    "void main(void) {"
        "gl_Position = vec4(coord.xy, 0, 1);"
        "texpos = coord.zw;"
    "}";

const char *FontFrag = "varying vec2 texpos;"
    "uniform sampler2D tex;"
    "uniform vec4 color;"
    "void main(void) {"
        "gl_FragColor = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;"
    "}";