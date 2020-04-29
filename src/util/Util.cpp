#include "Util.h"

#include <stdlib.h> 
#include <iostream>

namespace OpenRTP
{
    class Util::Impl
    {
    public:
        void Init()
        {
            ColorInit();
        }

        glm::vec4 GetUniqueColor()
        {
            if(PrivateColorVector.empty())
            {
                std::map<std::string, glm::vec4>::iterator It = ColorMap.begin();
                while(It != ColorMap.end())
                {
                    PrivateColorVector.push_back(It->second);
                    It++;
                }
            }

            int Num = std::rand() % PrivateColorVector.size();

            glm::vec4 Ret = PrivateColorVector[Num];

            PrivateColorVector.erase(PrivateColorVector.begin() + Num);

            return Ret;
        }

        GLuint CreateProgram(const char* VertexString, const char* FragmentString)
        {
            GLint Prog = glCreateProgram();
            GLuint Shader;

            if(VertexString != NULL)
            {
                Shader = CreateShader(VertexString, GL_VERTEX_SHADER);
                if(!Shader)
                {
                    std::cout << "Error, failed to create Vertex Shader" << std::endl;
                    return 0;
                }
                glAttachShader(Prog, Shader);
            }

            if(VertexString != NULL)
            {
                Shader = CreateShader(FragmentString, GL_FRAGMENT_SHADER);
                if(!Shader)
                {
                    std::cout << "Error, failed to create Fragment Shader" << std::endl;
                    return 0;
                }
                glAttachShader(Prog, Shader);
            }

            glLinkProgram(Prog);
            GLint Link = GL_FALSE;
            glGetProgramiv(Prog, GL_LINK_STATUS, &Link);
            if(!Link)
            {
                std::cout << "Error, failed to link program" << std::endl;
                glDeleteProgram(Prog);
                return 0;
            }

            return Prog;
        }

        GLint GetAttrib(GLuint Program, const char* Name)
        {
            GLint Attrib = glGetAttribLocation(Program, Name);
            if(Attrib == -1)
                std::cout << "Error, Couldn't bind attrib: " << Name << std::endl;
            return Attrib;
        }

        GLint GetUniform(GLuint Program, const char* Name)
        {
            GLint Uniform = glGetUniformLocation(Program, Name);
            if(Uniform == -1)
                std::cout << "Error, Couldn't bind uniform: " << Name << std::endl;
            return Uniform;
        }
    private:
        std::map<std::string, glm::vec4> ColorMap;
        std::vector<glm::vec4> PrivateColorVector;
        void ColorInit()
        {
            ColorMap.insert(std::make_pair("aqua", glm::vec4(0.000f, 1.000f, 1.000f, 1.0f)));
            ColorMap.insert(std::make_pair("aquamarine", glm::vec4(0.498f, 1.000f, 0.831f, 1.0f)));
            ColorMap.insert(std::make_pair("blue", glm::vec4(0.000f, 0.000f, 1.000f, 1.0f)));
            ColorMap.insert(std::make_pair("blueviolet", glm::vec4(0.541f, 0.169f, 0.886f, 1.0f)));
            ColorMap.insert(std::make_pair("brown", glm::vec4(0.647f, 0.165f, 0.165f, 1.0f)));
            ColorMap.insert(std::make_pair("burlywood", glm::vec4(0.871f, 0.722f, 0.529f, 1.0f)));
            ColorMap.insert(std::make_pair("cadetblue", glm::vec4(0.373f, 0.620f, 0.627f, 1.0f)));
            ColorMap.insert(std::make_pair("chartreuse", glm::vec4(0.498f, 1.000f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("chocolate", glm::vec4(0.824f, 0.412f, 0.118f, 1.0f)));
            ColorMap.insert(std::make_pair("coral", glm::vec4(1.000f, 0.498f, 0.314f, 1.0f)));
            ColorMap.insert(std::make_pair("cornflowerblue", glm::vec4(0.392f, 0.584f, 0.929f, 1.0f)));
            ColorMap.insert(std::make_pair("crimson", glm::vec4(0.863f, 0.078f, 0.235f, 1.0f)));
            ColorMap.insert(std::make_pair("cyan", glm::vec4(0.000f, 1.000f, 1.000f, 1.0f)));
            ColorMap.insert(std::make_pair("darkblue", glm::vec4(0.000f, 0.000f, 0.545f, 1.0f)));
            ColorMap.insert(std::make_pair("darkcyan", glm::vec4(0.000f, 0.545f, 0.545f, 1.0f)));
            ColorMap.insert(std::make_pair("darkgoldenrod", glm::vec4(0.722f, 0.525f, 0.043f, 1.0f)));
            ColorMap.insert(std::make_pair("darkgreen", glm::vec4(0.000f, 0.392f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("darkgrey", glm::vec4(0.663f, 0.663f, 0.663f, 1.0f)));
            ColorMap.insert(std::make_pair("darkkhaki", glm::vec4(0.741f, 0.718f, 0.420f, 1.0f)));
            ColorMap.insert(std::make_pair("darkmagenta", glm::vec4(0.545f, 0.000f, 0.545f, 1.0f)));
            ColorMap.insert(std::make_pair("darkolivegreen", glm::vec4(0.333f, 0.420f, 0.184f, 1.0f)));
            ColorMap.insert(std::make_pair("darkorange", glm::vec4(1.000f, 0.549f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("darkorchid", glm::vec4(0.600f, 0.196f, 0.800f, 1.0f)));
            ColorMap.insert(std::make_pair("darkred", glm::vec4(0.545f, 0.000f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("darksalmon", glm::vec4(0.914f, 0.588f, 0.478f, 1.0f)));
            ColorMap.insert(std::make_pair("darkseagreen", glm::vec4(0.561f, 0.737f, 0.561f, 1.0f)));
            ColorMap.insert(std::make_pair("darkslateblue", glm::vec4(0.282f, 0.239f, 0.545f, 1.0f)));
            ColorMap.insert(std::make_pair("darkslategray", glm::vec4(0.184f, 0.310f, 0.310f, 1.0f)));
            ColorMap.insert(std::make_pair("darkslategrey", glm::vec4(0.184f, 0.310f, 0.310f, 1.0f)));
            ColorMap.insert(std::make_pair("darkturquoise", glm::vec4(0.000f, 0.808f, 0.820f, 1.0f)));
            ColorMap.insert(std::make_pair("darkviolet", glm::vec4(0.580f, 0.000f, 0.827f, 1.0f)));
            ColorMap.insert(std::make_pair("deeppink", glm::vec4(1.000f, 0.078f, 0.576f, 1.0f)));
            ColorMap.insert(std::make_pair("deepskyblue", glm::vec4(0.000f, 0.749f, 1.000f, 1.0f)));
            ColorMap.insert(std::make_pair("dimgrey", glm::vec4(0.412f, 0.412f, 0.412f, 1.0f)));
            ColorMap.insert(std::make_pair("dodgerblue", glm::vec4(0.118f, 0.565f, 1.000f, 1.0f)));
            ColorMap.insert(std::make_pair("firebrick", glm::vec4(0.698f, 0.133f, 0.133f, 1.0f)));
            ColorMap.insert(std::make_pair("forestgreen", glm::vec4(0.133f, 0.545f, 0.133f, 1.0f)));
            ColorMap.insert(std::make_pair("fuchsia", glm::vec4(1.000f, 0.000f, 1.000f, 1.0f)));
            ColorMap.insert(std::make_pair("gold", glm::vec4(1.000f, 0.843f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("goldenrod", glm::vec4(0.855f, 0.647f, 0.125f, 1.0f)));
            ColorMap.insert(std::make_pair("gray", glm::vec4(0.502f, 0.502f, 0.502f, 1.0f)));
            ColorMap.insert(std::make_pair("green", glm::vec4(0.000f, 0.502f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("greenyellow", glm::vec4(0.678f, 1.000f, 0.184f, 1.0f)));
            ColorMap.insert(std::make_pair("grey", glm::vec4(0.502f, 0.502f, 0.502f, 1.0f)));
            ColorMap.insert(std::make_pair("hotpink", glm::vec4(1.000f, 0.412f, 0.706f, 1.0f)));
            ColorMap.insert(std::make_pair("indianred", glm::vec4(0.804f, 0.361f, 0.361f, 1.0f)));
            ColorMap.insert(std::make_pair("indigo", glm::vec4(0.294f, 0.000f, 0.510f, 1.0f)));
            ColorMap.insert(std::make_pair("khaki", glm::vec4(0.941f, 0.902f, 0.549f, 1.0f)));
            ColorMap.insert(std::make_pair("lavender", glm::vec4(0.902f, 0.902f, 0.980f, 1.0f)));
            ColorMap.insert(std::make_pair("lavenderblush", glm::vec4(1.000f, 0.941f, 0.961f, 1.0f)));
            ColorMap.insert(std::make_pair("lawngreen", glm::vec4(0.486f, 0.988f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("lightblue", glm::vec4(0.678f, 0.847f, 0.902f, 1.0f)));
            ColorMap.insert(std::make_pair("lightcoral", glm::vec4(0.941f, 0.502f, 0.502f, 1.0f)));
            ColorMap.insert(std::make_pair("lightgreen", glm::vec4(0.565f, 0.933f, 0.565f, 1.0f)));
            ColorMap.insert(std::make_pair("lightgrey", glm::vec4(0.827f, 0.827f, 0.827f, 1.0f)));
            ColorMap.insert(std::make_pair("lightpink", glm::vec4(1.000f, 0.714f, 0.757f, 1.0f)));
            ColorMap.insert(std::make_pair("lightsalmon", glm::vec4(1.000f, 0.627f, 0.478f, 1.0f)));
            ColorMap.insert(std::make_pair("lightseagreen", glm::vec4(0.125f, 0.698f, 0.667f, 1.0f)));
            ColorMap.insert(std::make_pair("lightskyblue", glm::vec4(0.529f, 0.808f, 0.980f, 1.0f)));
            ColorMap.insert(std::make_pair("lightsteelblue", glm::vec4(0.690f, 0.769f, 0.871f, 1.0f)));
            ColorMap.insert(std::make_pair("lime", glm::vec4(0.000f, 1.000f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("limegreen", glm::vec4(0.196f, 0.804f, 0.196f, 1.0f)));
            ColorMap.insert(std::make_pair("magenta", glm::vec4(1.000f, 0.000f, 1.000f, 1.0f)));
            ColorMap.insert(std::make_pair("maroon", glm::vec4(0.502f, 0.000f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("mediumaquamarine", glm::vec4(0.400f, 0.804f, 0.667f, 1.0f)));
            ColorMap.insert(std::make_pair("mediumblue", glm::vec4(0.000f, 0.000f, 0.804f, 1.0f)));
            ColorMap.insert(std::make_pair("mediumorchid", glm::vec4(0.729f, 0.333f, 0.827f, 1.0f)));
            ColorMap.insert(std::make_pair("mediumpurple", glm::vec4(0.576f, 0.439f, 0.859f, 1.0f)));
            ColorMap.insert(std::make_pair("mediumseagreen", glm::vec4(0.235f, 0.702f, 0.443f, 1.0f)));
            ColorMap.insert(std::make_pair("mediumslateblue", glm::vec4(0.482f, 0.408f, 0.933f, 1.0f)));
            ColorMap.insert(std::make_pair("mediumspringgreen", glm::vec4(0.000f, 0.980f, 0.604f, 1.0f)));
            ColorMap.insert(std::make_pair("mediumturquoise", glm::vec4(0.282f, 0.820f, 0.800f, 1.0f)));
            ColorMap.insert(std::make_pair("mediumvioletred", glm::vec4(0.780f, 0.082f, 0.522f, 1.0f)));
            ColorMap.insert(std::make_pair("midnightblue", glm::vec4(0.098f, 0.098f, 0.439f, 1.0f)));
            ColorMap.insert(std::make_pair("mistyrose", glm::vec4(1.000f, 0.894f, 0.882f, 1.0f)));
            ColorMap.insert(std::make_pair("moccasin", glm::vec4(1.000f, 0.894f, 0.710f, 1.0f)));
            ColorMap.insert(std::make_pair("navajowhite", glm::vec4(1.000f, 0.871f, 0.678f, 1.0f)));
            ColorMap.insert(std::make_pair("navy", glm::vec4(0.000f, 0.000f, 0.502f, 1.0f)));
            ColorMap.insert(std::make_pair("olive", glm::vec4(0.502f, 0.502f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("olivedrab", glm::vec4(0.420f, 0.557f, 0.137f, 1.0f)));
            ColorMap.insert(std::make_pair("orange", glm::vec4(1.000f, 0.647f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("orangered", glm::vec4(1.000f, 0.271f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("orchid", glm::vec4(0.855f, 0.439f, 0.839f, 1.0f)));
            ColorMap.insert(std::make_pair("palegoldenrod", glm::vec4(0.933f, 0.910f, 0.667f, 1.0f)));
            ColorMap.insert(std::make_pair("palegreen", glm::vec4(0.596f, 0.984f, 0.596f, 1.0f)));
            ColorMap.insert(std::make_pair("paleturquoise", glm::vec4(0.686f, 0.933f, 0.933f, 1.0f)));
            ColorMap.insert(std::make_pair("palevioletred", glm::vec4(0.859f, 0.439f, 0.576f, 1.0f)));
            ColorMap.insert(std::make_pair("peachpuff", glm::vec4(1.000f, 0.855f, 0.725f, 1.0f)));
            ColorMap.insert(std::make_pair("peru", glm::vec4(0.804f, 0.522f, 0.247f, 1.0f)));
            ColorMap.insert(std::make_pair("pink", glm::vec4(1.000f, 0.753f, 0.796f, 1.0f)));
            ColorMap.insert(std::make_pair("plum", glm::vec4(0.867f, 0.627f, 0.867f, 1.0f)));
            ColorMap.insert(std::make_pair("powderblue", glm::vec4(0.690f, 0.878f, 0.902f, 1.0f)));
            ColorMap.insert(std::make_pair("purple", glm::vec4(0.502f, 0.000f, 0.502f, 1.0f)));
            ColorMap.insert(std::make_pair("red", glm::vec4(1.000f, 0.000f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("rosybrown", glm::vec4(0.737f, 0.561f, 0.561f, 1.0f)));
            ColorMap.insert(std::make_pair("royalblue", glm::vec4(0.255f, 0.412f, 0.882f, 1.0f)));
            ColorMap.insert(std::make_pair("saddlebrown", glm::vec4(0.545f, 0.271f, 0.075f, 1.0f)));
            ColorMap.insert(std::make_pair("salmon", glm::vec4(0.980f, 0.502f, 0.447f, 1.0f)));
            ColorMap.insert(std::make_pair("sandybrown", glm::vec4(0.957f, 0.643f, 0.376f, 1.0f)));
            ColorMap.insert(std::make_pair("seagreen", glm::vec4(0.180f, 0.545f, 0.341f, 1.0f)));
            ColorMap.insert(std::make_pair("sienna", glm::vec4(0.627f, 0.322f, 0.176f, 1.0f)));
            ColorMap.insert(std::make_pair("silver", glm::vec4(0.753f, 0.753f, 0.753f, 1.0f)));
            ColorMap.insert(std::make_pair("skyblue", glm::vec4(0.529f, 0.808f, 0.922f, 1.0f)));
            ColorMap.insert(std::make_pair("slateblue", glm::vec4(0.416f, 0.353f, 0.804f, 1.0f)));
            ColorMap.insert(std::make_pair("slategray", glm::vec4(0.439f, 0.502f, 0.565f, 1.0f)));
            ColorMap.insert(std::make_pair("slategrey", glm::vec4(0.439f, 0.502f, 0.565f, 1.0f)));
            ColorMap.insert(std::make_pair("springgreen", glm::vec4(0.000f, 1.000f, 0.498f, 1.0f)));
            ColorMap.insert(std::make_pair("steelblue", glm::vec4(0.275f, 0.510f, 0.706f, 1.0f)));
            ColorMap.insert(std::make_pair("tan", glm::vec4(0.824f, 0.706f, 0.549f, 1.0f)));
            ColorMap.insert(std::make_pair("teal", glm::vec4(0.000f, 0.502f, 0.502f, 1.0f)));
            ColorMap.insert(std::make_pair("tomato", glm::vec4(1.000f, 0.388f, 0.278f, 1.0f)));
            ColorMap.insert(std::make_pair("turquoise", glm::vec4(0.251f, 0.878f, 0.816f, 1.0f)));
            ColorMap.insert(std::make_pair("violet", glm::vec4(0.933f, 0.510f, 0.933f, 1.0f)));
            ColorMap.insert(std::make_pair("wheat", glm::vec4(0.961f, 0.871f, 0.702f, 1.0f)));
            ColorMap.insert(std::make_pair("yellow", glm::vec4(1.000f, 1.000f, 0.000f, 1.0f)));
            ColorMap.insert(std::make_pair("yellowgreen", glm::vec4(0.604f, 0.804f, 0.196f, 1.0f)));
        }

        GLint CreateShader(const char* ShaderString, GLenum Type)
        {
            if(ShaderString == NULL)
            {
                std::cout << "Error, invalid shader string" << std::endl;
                return 0;
            }

            GLuint Res = glCreateShader(Type);
            const GLchar* ShaderSource[] = {
                #ifdef GL_ES_VERSION_2_0
                    "#version 100\n"  // OpenGL ES 2.0
                #else
                    "#version 120\n"  // OpenGL 2.1
                #endif
                    ,
                #ifdef GL_ES_VERSION_2_0
                    // Define default float precision for fragment shaders:
                    (type == GL_FRAGMENT_SHADER) ?
                    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
                    "precision highp float;           \n"
                    "#else                            \n"
                    "precision mediump float;         \n"
                    "#endif                           \n"
                    : ""
                #else
                    "#define lowp   \n"
                    "#define mediump\n"
                    "#define highp  \n"
                #endif
                    ,
                    ShaderString
            };
            glShaderSource(Res, 3, ShaderSource, NULL);

            glCompileShader(Res);
            GLint Compile = GL_FALSE;
            glGetShaderiv(Res, GL_COMPILE_STATUS, &Compile);
            if (Compile == GL_FALSE)
            {
                std::cout << "Error, failed to create shader" << std::endl;
                glDeleteShader(Res);
                return 0;
            }

            return Res;
        }
    };

    Util::Util() : PImpl{std::make_unique<Impl>()}
    {
        PImpl->Init();
    }

    Util::~Util() = default;

    glm::vec4 Util::GetUniqueColor() { return PImpl->GetUniqueColor(); }
    GLuint Util::CreateProgram(const char* VertexString, const char* FragmentString) { return PImpl->CreateProgram(VertexString, FragmentString); }
    GLint Util::GetAttrib(GLuint Program, const char* Name) { return PImpl->GetAttrib(Program, Name); }
    GLint Util::GetUniform(GLuint Program, const char* Name) { return PImpl->GetUniform(Program, Name); }

}