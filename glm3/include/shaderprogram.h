#pragma once

#include <iostream>
#include <vector>
#include <GL/glew.h>

#include "glm3_export.h"

class GLM3_EXPORT ShaderProgram
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
