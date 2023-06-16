#pragma once

#include <GL/glew.h>

class GLHelper
{
public:
    bool initGlew();
    void render();
    void setSize(int width, int height);

    bool initData();
    void cleanup();

private:
    GLuint mVbo, mVao, mShaderProgram;
};
