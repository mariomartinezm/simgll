#pragma once

#include <GL/glew.h>
#include "shaderprogram.h"

class GLHelper
{
public:
    bool initGlew();
    void render();
    void setSize(int width, int height);

    bool initData();
    void cleanup();

private:
    GLuint mVao  = { 0 };
    GLuint mVbo  = { 0 };
    ShaderProgram* mShaderProgram;
};
