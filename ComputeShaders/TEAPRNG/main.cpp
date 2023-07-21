#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaderprogram.h"

constexpr GLuint WIDTH = 512, HEIGHT = 512;
constexpr GLuint TEXTURE_WIDTH = 512, TEXTURE_HEIGHT = 512;

void error_callback(GLint error, const GLchar* description);
void createQuad(GLuint& vao, GLuint& vbo, GLuint& ebo);
GLuint createTextureObject(GLuint width, GLuint height);

int main()
{
    srand(time(nullptr));

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
        std::cerr << "GLFW Error: Can't create window.\n";

        glfwTerminate();

        exit(1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK)
    {
        std::cerr << "GLEW error: " << glewGetErrorString(status) << "\n";

        glfwTerminate();

        exit(1);
    }

    ShaderProgram computeProgram;
    computeProgram.addShader("compute_shader.glsl", GL_COMPUTE_SHADER);
    computeProgram.compile();

    GLint cpuSeedLocation = computeProgram.getLocation("cpuSeed");

    ShaderProgram renderProgram;
    renderProgram.addShader("vertex_shader.glsl", GL_VERTEX_SHADER);
    renderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    renderProgram.compile();

    GLuint vao, vbo, ebo;
    createQuad(vao, vbo, ebo);

    GLuint textures[2], ping, pong;

    textures[0] = createTextureObject(TEXTURE_WIDTH, TEXTURE_HEIGHT);
    textures[1] = createTextureObject(TEXTURE_WIDTH, TEXTURE_HEIGHT);

    ping = textures[0];
    pong = textures[1];

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while(!glfwWindowShouldClose(window))
    {
        computeProgram.use();

        // Bind input and output images
        glBindImageTexture(0, ping, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
        glBindImageTexture(1, pong, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

        glUniform1ui(cpuSeedLocation, rand());

        glDispatchCompute(TEXTURE_WIDTH / 32, TEXTURE_HEIGHT / 32, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
                        GL_TEXTURE_FETCH_BARRIER_BIT);

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        renderProgram.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ping);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                       static_cast<GLvoid*>(nullptr));
        glBindVertexArray(0);

        glfwSwapBuffers(window);

        GLuint temp = ping;
        ping = pong;
        pong = temp;
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwTerminate();

    return 0;
}

void error_callback(GLint error, const GLchar* description)
{
    std::cerr << "GLFW error " << error << ": " << description << "\n";

    glfwTerminate();

    exit(1);
}

void createQuad(GLuint& vao, GLuint& vbo, GLuint& ebo)
{
    GLfloat vertices[] =
    {
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f,
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(GL_FLOAT), static_cast<GLvoid*>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(GL_FLOAT), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

GLuint createTextureObject(GLuint width, GLuint height)
{
    GLuint* textureData = new GLuint[width * height];

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, width, height, 0, GL_RGBA_INTEGER,
                 GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] textureData;

    return texture;
}
