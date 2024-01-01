#pragma once

#include <iostream>
#include <vector>
#include <GL/glew.h>

#include "simgll_export.h"

class SIMGLL_EXPORT ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    GLuint name() const;

    GLvoid addShader(const std::string& filename, const GLenum& shaderType);
    GLvoid compile();

    GLint getLocation(const std::string& name) const;
    GLvoid use();

private:
    GLuint mProgramName = { 0 };
    std::vector<GLuint> mShaderObjects;
};
