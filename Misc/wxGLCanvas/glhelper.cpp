#include "glhelper.h"
#include "shaderprogram.h"

bool GLHelper::initGlew()
{
    glewExperimental = true;
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

    ShaderProgram shaderProgram;
    shaderProgram.addShader("vertex_shader.glsl",   GL_VERTEX_SHADER);
    shaderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    shaderProgram.compile();
    mShaderProgram = shaderProgram.name();
    std::cout << "Shader = " << mShaderProgram << std::endl;

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

    return true;
}

void GLHelper::cleanup()
{
    glDeleteVertexArrays(1, &mVao);
    glDeleteBuffers(1, &mVbo);
    glDeleteProgram(mShaderProgram);
}

void GLHelper::render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(mShaderProgram);
    std::cout << "Shader = " << mShaderProgram << std::endl;

    glBindVertexArray(mVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void GLHelper::setSize(int width, int height)
{
    glViewport(0, 0, width, height);
}
