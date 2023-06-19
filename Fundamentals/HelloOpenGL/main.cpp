#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaderprogram.h"

constexpr GLuint WIDTH = 512, HEIGHT = 512;

int main()
{
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

    GLenum result = glewInit();
    if(result != GLEW_OK)
    {
        std::cout << "Glew Error: " << glewGetErrorString(result) << std::endl;
        glfwTerminate();

        return 1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    GLuint programName;
    ShaderProgram shaderProgram(programName);
    shaderProgram.addShader("vertex_shader.glsl", GL_VERTEX_SHADER);
    shaderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    shaderProgram.compile();

    GLfloat vertices[] =
    {
         -0.5f, -0.5f,  0.0f,/*   1.0f, 0.0f, 0.0f, 1.0f,*/
          0.5f, -0.5f,  0.0f,/*   0.0f, 1.0f, 0.0f, 1.0f,*/
          0.0f,  0.5f,  0.0f,/*   0.0f, 0.0f, 1.0f, 1.0f,*/
    };

    GLuint vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(programName);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(programName);

    glfwTerminate();

    return 0;
}
