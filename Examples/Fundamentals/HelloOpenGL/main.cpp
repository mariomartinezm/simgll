#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "shaderprogram.h"

constexpr GLuint WIDTH = 512, HEIGHT = 512;

struct Vertex
{
    glm::vec3 position;
    glm::vec4 color;
};

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

    simgll::ShaderProgram renderProgram;
    renderProgram.addShader("vertex_shader.glsl", GL_VERTEX_SHADER);
    renderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    renderProgram.compile();

    std::vector<Vertex> vertices = {
        Vertex{ {-0.5F, -0.5F,  0.0F}, {1.0F, 0.0F, 0.0F, 1.0F} },
        Vertex{ { 0.5F, -0.5F,  0.0F}, {0.0F, 1.0F, 0.0F, 1.0F} },
        Vertex{ { 0.0F,  0.5F,  0.0F}, {0.0F, 0.0F, 1.0F, 1.0F} },
    };

    GLuint vao, vbo;

    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, vertices.size() * sizeof(Vertex),
                         vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    glCreateVertexArrays(1, &vao);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);

    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);

    glBindVertexArray(0);

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        renderProgram.use();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();

    return 0;
}
