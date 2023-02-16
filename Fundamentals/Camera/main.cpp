#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderprogram.h"
#include "camera.h"

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
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;

    GLenum result = glewInit();
    if(result != GLEW_OK)
    {
        std::cout << "Glew Error: " << glewGetErrorString(result) << std::endl;
        glfwTerminate();

        return 1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    ShaderProgram shaderProgram;
    shaderProgram.addShader("vertex_shader.glsl", GL_VERTEX_SHADER);
    shaderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    shaderProgram.compile();

    GLint mvpLocation = glGetUniformLocation(shaderProgram.name(), "mvp");

    Camera camera;
    camera.position = {0.0f, 0.5f,  3.5f};
    camera.target   = {0.0f, 0.0f, -1.0f};
    camera.up       = {0.0f, 1.0f,  0.0f};

    glfwSetWindowUserPointer(window, (GLvoid*)&camera);

    GLfloat vertices[] =
    {
         -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f, 1.0f,
          0.0f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f, 1.0f,
          0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f, 1.0f,
          0.0f,  0.5f,  0.0f,   1.0f, 0.0f, 1.0f, 1.0f
    };

    GLuint elements[] =
    {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        1, 2, 0
    };

    GLuint vbo, vao, ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            7 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
            7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

    GLfloat startTime = 0.0F;
    GLfloat oldTime   = 0.0F;
    GLfloat deltaTime = 0.0F;

    while(!glfwWindowShouldClose(window))
    {
        startTime = (GLfloat)glfwGetTime();
        deltaTime = startTime - oldTime;
        oldTime   = startTime;

        glfwPollEvents();
        poll_keyboard(window, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 newTarget = camera.position + camera.target;
        auto mv_matrix = glm::lookAt(camera.position,
                                     newTarget,
                                     camera.up);

        auto proj_matrix = glm::perspective(45.0F,
                                            (float)(WIDTH) / HEIGHT,
                                            0.1f,
                                            100.0f);

        auto mvp = proj_matrix * mv_matrix;

        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

        glUseProgram(shaderProgram.name());
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwTerminate();

    return 0;
}
