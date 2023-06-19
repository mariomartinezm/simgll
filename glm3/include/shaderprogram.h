#pragma once

#include <iostream>
#include <vector>
#include <GL/glew.h>

#include "glm3_export.h"

class GLM3_EXPORT ShaderProgram
{
public:
    ShaderProgram(GLuint& programName);
    ~ShaderProgram();

    GLvoid setProgramName(GLuint& programName);
    GLvoid addShader(const std::string& filename, const GLenum& shaderType);
    GLvoid compile();

private:
    GLuint* mProgramName;
    std::vector<GLuint> mShaderObjects;
};
