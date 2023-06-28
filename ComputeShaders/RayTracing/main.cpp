#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include "shaderprogram.h"
#include "camera.h"

constexpr GLuint WIDTH          = 512;
constexpr GLuint HEIGHT         = 512;
constexpr GLuint TEXTURE_WIDTH  = 512;
constexpr GLuint TEXTURE_HEIGHT = 512;

GLvoid error_callback(GLint error, const GLchar* description);
GLvoid createQuad(GLuint& vao, GLuint& vbo, GLuint& ebo);
GLuint createTextureObject(GLuint width, GLuint height);

void poll_keyboard(GLFWwindow* window);

int main()
{
    glfwSetErrorCallback(error_callback);

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL App", nullptr,
                                          nullptr);

    if(!window)
    {
        std::cerr << "GLFW Error: Can't create window.\n";

        glfwTerminate();

        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK)
    {
        std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";

        glfwTerminate();

        exit(1);
    }

    Camera camera(window,
                  glm::vec3{ 0.0f, 0.0f,  3.0f },
                  glm::vec3{ 0.0f, 0.0f, -1.0f },
                  glm::vec3{ 0.0f, 1.0f,  0.0f });

    GLuint computeProgram;
    ShaderProgram shaderProgram(computeProgram);
    shaderProgram.addShader("compute_shader.glsl", GL_COMPUTE_SHADER);
    shaderProgram.compile();

    GLint cpuSeedLocation      = glGetUniformLocation(computeProgram, "cpuSeed");
    GLint cameraPosLocation    = glGetUniformLocation(computeProgram, "cameraPos");
    GLint cameraTargetLocation = glGetUniformLocation(computeProgram, "cameraTarget");

    GLuint renderProgram;
    shaderProgram.setProgramName(renderProgram);
    shaderProgram.addShader("vertex_shader.glsl",   GL_VERTEX_SHADER);
    shaderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    shaderProgram.compile();

    GLuint vao, vbo, ebo;
    createQuad(vao, vbo, ebo);

    GLuint texture;

    texture = createTextureObject(TEXTURE_WIDTH, TEXTURE_HEIGHT);

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

    GLfloat currentTime = 0.0F;
    GLfloat oldTime     = 0.0F;
    GLfloat deltaTime   = 0.0F;

    while(!glfwWindowShouldClose(window))
    {
        currentTime = (GLfloat) glfwGetTime();
        deltaTime   = currentTime - oldTime;
        oldTime     = currentTime;

        auto newTarget = camera.position() + camera.target();

        glUseProgram(computeProgram);

        glUniform1ui(cpuSeedLocation, rand());
        glUniform3f(cameraPosLocation, camera.position().x, camera.position().y, camera.position().z);
        glUniform3f(cameraTargetLocation, newTarget.x, newTarget.y, newTarget.z);

        glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        glDispatchCompute(TEXTURE_WIDTH / 32, TEXTURE_HEIGHT / 32, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        glfwPollEvents();
        camera.update(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(renderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                       static_cast<GLvoid*>(nullptr));
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

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
    GLfloat vertices[] =
    {
        -1.0F, -1.0F, 0.0F, 0.0F,
         1.0F, -1.0F, 1.0F, 0.0F,
         1.0F,  1.0F, 1.0F, 1.0F,
        -1.0F,  1.0F, 0.0F, 1.0F,
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                          static_cast<GLvoid*>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

GLuint createTextureObject(GLuint width, GLuint height)
{
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
                 GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}
