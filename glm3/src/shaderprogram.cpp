#include <fstream>
#include <sstream>
#include "shaderprogram.h"

ShaderProgram::ShaderProgram()
{
    mName = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(mName);
}

GLuint ShaderProgram::name()
{
    return mName;
}

GLvoid ShaderProgram::addShader(const std::string& filename, const GLenum& shaderType)
{
    std::ifstream fs(filename);

    if(!fs)
    {
        std::cerr << "Can't find " << filename << std::endl;

        exit(1);
    }

    std::stringstream code;
    code << fs.rdbuf();
    fs.close();

    GLuint shaderObject = glCreateShader(shaderType);

    if(shaderObject == 0)
    {
        std::cerr << "Error creating shader type: " << shaderType << std::endl;

        exit(1);
    }

    std::string codeString = code.str();
    const GLchar* codePtr = codeString.c_str(); 

    glShaderSource(shaderObject, 1, &codePtr, nullptr);
    glCompileShader(shaderObject);

    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shaderObject, 512, nullptr, infoLog);

        std::cerr << "Error compiling shader type: " << shaderType << std::endl;
        std::cerr << infoLog << std::endl;

        exit(1);
    }

    mShaderObjects.push_back(shaderObject);

    glAttachShader(mName, shaderObject);
}

GLvoid ShaderProgram::compile()
{
    glLinkProgram(mName);

    GLint success;
    GLchar infoLog[512];

    glGetProgramiv(mName, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(mName, 512, nullptr, infoLog);

        std::cerr << "Link Error: " << infoLog << std::endl;

        exit(1);
    }

    for(const auto& shaderObject: mShaderObjects)
    {
        glDeleteShader(shaderObject);
    }
}
