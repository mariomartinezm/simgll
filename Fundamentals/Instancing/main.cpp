#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderprogram.h"
#include "camera.h"

constexpr GLuint WIDTH = 512, HEIGHT = 512;

void error_cb(GLint error, const GLchar* description);

int main()
{
    glfwSetErrorCallback(error_cb);

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL App",
                                          nullptr, nullptr);

    if(!window)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK)
    {
        std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";

        return 1;
    }

    const GLfloat grassBlade[] =
    {
        -0.3f,  0.0f,
         0.3f,  0.0f,
        -0.2f,  1.0f,
         0.1f,  1.3f,
        -0.05f, 2.3f,
         0.0f,  3.3f
    };

    GLuint grassBuffer;
    glGenBuffers(1, &grassBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, grassBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassBlade), grassBlade, GL_STATIC_DRAW);

    GLuint grassVao;
    glGenVertexArrays(1, &grassVao);
    glBindVertexArray(grassVao);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    GLuint grassProgram;
    ShaderProgram shaderProgram(grassProgram);
    shaderProgram.addShader("vertex_shader.glsl",   GL_VERTEX_SHADER);
    shaderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    shaderProgram.compile();

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glActiveTexture(GL_TEXTURE1);
    glActiveTexture(GL_TEXTURE2);
    glActiveTexture(GL_TEXTURE3);
    glActiveTexture(GL_TEXTURE4);


    while(!glfwWindowShouldClose(window))
    {
        GLfloat t = static_cast<GLfloat>(glfwGetTime()) * 0.02f;
        GLfloat r = 550.0f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();

        auto mv = glm::lookAt(glm::vec3(sinf(t) * r, 25.0f, cosf(t) * r),
                              glm::vec3(0.0f, -50.0f, 0.0f),
                              glm::vec3(0.0f, 1.0f, 0.0f));
        auto prj = glm::perspective(45.0f, (GLfloat)(WIDTH) / HEIGHT, 0.1f, 1000.0f);
        auto mvp = prj * mv;

        glUseProgram(grassProgram);
        glUniformMatrix4fv(glGetUniformLocation(grassProgram, "mvp"),
                           1, GL_FALSE, &mvp[0][0]);

        glBindVertexArray(grassVao);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, 1024 * 1024);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteProgram(grassProgram);

    return 0;
}

void error_cb(GLint error, const GLchar* description)
{
    std::cerr << "GLFW error " << error << ": " << description << "\n";
}
