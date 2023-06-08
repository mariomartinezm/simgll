#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <iostream>
#include <vector>
#include <GL/glew.h>

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    GLuint name();

    GLvoid addShader(const std::string& filename, const GLenum& shaderType);
    GLvoid compile();

private:
    GLuint mName;
    std::vector<GLuint> mShaderObjects;
};

#endif // SHADERPROGRAM_H
