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

    GLvoid addShader(const std::string& filename, const GLenum& shaderType);
    GLvoid compile();

    GLint getLocation(const std::string& name) const;
    GLvoid use();

private:
    GLuint mProgramName = { 0 };
    std::vector<GLuint> mShaderObjects;
};
