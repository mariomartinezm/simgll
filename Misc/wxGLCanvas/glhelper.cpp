#include "shaderprogram.h"

#ifdef __WXMSW__
    #include <GL/wglew.h>
#elif defined(__WXGTK__)
    #include <GL/glxew.h>
#endif

#include "glhelper.h"
#include "shaderprogram.h"

bool GLHelper::initGlew()
{
    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    return status == GLEW_OK;
}

bool GLHelper::initData()
{
    GLfloat points[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };

    mShaderProgram.addShader("vertex_shader.glsl",   GL_VERTEX_SHADER);
    mShaderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    mShaderProgram.compile();

    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mVbo);

    glBindVertexArray(mVao);

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

    return true;
}

void GLHelper::cleanup()
{
    glDeleteVertexArrays(1, &mVao);
    glDeleteBuffers(1, &mVbo);
}

void GLHelper::render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    mShaderProgram.use();

    glBindVertexArray(mVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void GLHelper::setSize(int width, int height)
{
    glViewport(0, 0, width, height);
}
