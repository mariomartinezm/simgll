#include <iostream>
#include <cstdlib>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaderprogram.h"

constexpr GLuint WIDTH  = 512;
constexpr GLuint HEIGHT = 512;
constexpr GLuint NUM_VERTICES_QUAD = 6;

GLvoid error_callback(GLint error, const GLchar* description);
GLvoid createQuad(GLuint& vao, GLuint& vbo, GLuint& ebo);

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
        std::cerr << "GLFW error: Can't create window\n";
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK)
    {
        std::cerr << "GLEW error" << glewGetErrorString(status) << "\n";
        glfwTerminate();
        exit(1);
    }

    GLuint renderProgram;
    ShaderProgram shaderProgram(renderProgram);
    shaderProgram.addShader("vertex_shader.glsl",   GL_VERTEX_SHADER);
    shaderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    shaderProgram.compile();

    GLuint vao; GLuint vbo; GLuint ebo;
    createQuad(vao, vbo, ebo);

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Render
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(renderProgram);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, NUM_VERTICES_QUAD, GL_UNSIGNED_INT,
                       static_cast<GLvoid*>(nullptr));
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    // Release objects
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(renderProgram);

    glfwTerminate();

    return 0;
}

GLvoid error_callback(GLint error, const GLchar* description)
{
    std::cerr << "GLFW error " << error << ": " << description << "\n";
    exit(1);
}

GLvoid createQuad(GLuint& vao, GLuint& vbo, GLuint& ebo)
{
    std::vector<GLfloat> vertices =
    {
        -1.0F, -1.0F,   1.0F, 0.0F, 0.0F, 1.0F,
         1.0F, -1.0F,   0.0F, 1.0F, 0.0F, 1.0F,
         1.0F,  1.0F,   0.0F, 0.0F, 1.0F, 1.0F,
        -1.0F,  1.0F,   1.0F, 0.0F, 1.0F, 1.0F,
    };

    std::vector<GLuint> elements =
    {
        0, 1, 3,
        1, 2, 3,
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint),
                 elements.data(), GL_STATIC_DRAW);

    // vertex format description
    glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

    // Both attributes will get their data from the buffer at binding index 0
    glVertexAttribBinding(0, 0); // Position
    glVertexAttribBinding(1, 0); // Color

    // Bind vbo to binding index 0
    glBindVertexBuffer(0, vbo, 0, 6 * sizeof(GLfloat));

    // Enable attribute access
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}
