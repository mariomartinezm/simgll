#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaderprogram.h"

constexpr GLuint WIDTH = 512, HEIGHT = 512;
constexpr GLuint TEXTURE_WIDTH = 32, TEXTURE_HEIGHT = 32;

void error_callback(GLint error, const GLchar* description);
void createModel(GLuint& vao, GLuint& vbo, GLuint& ebo);
GLuint createTextureObject(GLuint textureWidth, GLuint textureHeight,
                           bool initializeData);

int main()
{
    glfwSetErrorCallback(error_callback);

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL App",
                                          nullptr, nullptr);

    if(!window)
    {
        glfwTerminate();

        exit(1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK)
    {
        std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";

        exit(1);
    }

    ShaderProgram computeProgram;
    computeProgram.addShader("compute_shader.glsl", GL_COMPUTE_SHADER);
    computeProgram.compile();

    // Create input and output textures
    GLuint inputTexture  = createTextureObject(TEXTURE_WIDTH, TEXTURE_HEIGHT, true);
    GLuint outputTexture = createTextureObject(TEXTURE_WIDTH, TEXTURE_HEIGHT, false);

    // Create render program
    ShaderProgram renderProgram;
    renderProgram.addShader("vertex_shader.glsl",   GL_VERTEX_SHADER);
    renderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    renderProgram.compile();

    GLuint vao, vbo, ebo;
    createModel(vao, vbo, ebo);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLfloat currentTime  = 0.0f;
    GLfloat deltaTime    = 0.0f;
    GLfloat oldTime      = 0.0f;

    while(!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        deltaTime  += currentTime - oldTime;
        oldTime     = currentTime;

        // Bind input and output images
        // Since we have a plain one-dimensional texture level is 0
        glBindImageTexture(0, inputTexture,  0, GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA32F);
        glBindImageTexture(1, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        // Dispatch the compute shader
        glUseProgram(computeProgram.name());

        glUniform1i(glGetUniformLocation(computeProgram.name(), "imgInput"),  0);
        glUniform1i(glGetUniformLocation(computeProgram.name(), "imgOutput"), 1);

        glDispatchCompute(TEXTURE_WIDTH / 32, TEXTURE_HEIGHT / 32, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        if(static_cast<int>(deltaTime) >= 0.016)
        {
            std::swap(inputTexture, outputTexture);
            deltaTime = 0.0;
        }

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(renderProgram.name());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputTexture);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &inputTexture);
    glDeleteTextures(1, &outputTexture);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glDeleteVertexArrays(1, &vao);

    glfwTerminate();

    return 0;
}

void error_callback(GLint error, const GLchar* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << "\n";
}

void createModel(GLuint& vao, GLuint& vbo, GLuint& ebo)
{
    GLfloat vertices[] =
    {
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f,
    };

    GLuint elements[] =
    {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

GLuint createTextureObject(GLuint textureWidth, GLuint textureHeight,
                           bool initializeData)
{
    // Create texture data for a rgba32f texture
    GLfloat* textureData = new GLfloat[textureWidth * textureHeight * 4];
    GLfloat* p = textureData;

    if(initializeData)
    {
        for(GLuint row = 0; row < textureHeight; row++)
        {
            for(GLuint col = 0; col < textureWidth; col++)
            {
                if((row < 16 && col < 16) || (row >= 16 && col >= 16))
                {
                    *p++ = 1.0f;
                    *p++ = 0.0f;
                    *p++ = 0.0f;
                    *p++ = 1.0f;
                }
                else
                {
                    *p++ = 0.0f;
                    *p++ = 0.0f;
                    *p++ = 1.0f;
                    *p++ = 1.0f;
                }
            }
        }
    }

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if(initializeData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth,
                     textureHeight, 0, GL_RGBA, GL_FLOAT, textureData);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth,
                     textureHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] textureData;

    return texture;
}
