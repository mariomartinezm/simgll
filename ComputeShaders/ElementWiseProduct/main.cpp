#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaderprogram.h"

constexpr GLuint WIDTH = 512, HEIGHT = 512;
constexpr GLsizei NUM_ELEMENTS = 2048;

void error_callback(GLint error, const GLchar* description);
void elementWiseProduct(const std::vector<GLfloat>& a,
                          const std::vector<GLfloat>& b,
                          std::vector<GLfloat>& output);

int main()
{
    glfwSetErrorCallback(error_callback);

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Element Wise Product",
                                          nullptr, nullptr);

    if(!window)
    {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if (status != GLEW_OK)
    {
        std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
        glfwTerminate();
        exit(1);
    }

    GLuint computeProgram;
    ShaderProgram shaderProgram(computeProgram);
    shaderProgram.addShader("compute_shader.glsl", GL_COMPUTE_SHADER);
    shaderProgram.compile();

    GLuint dataBuffers[3];

    glGenBuffers(3, dataBuffers);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffers[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_ELEMENTS * sizeof(GLfloat),
                 nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffers[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_ELEMENTS * sizeof(GLfloat),
                 nullptr, GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffers[2]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_ELEMENTS * sizeof(GLfloat),
                 nullptr, GL_DYNAMIC_COPY);

    std::vector<GLfloat> inputDataA(NUM_ELEMENTS);
    std::vector<GLfloat> inputDataB(NUM_ELEMENTS);
    std::vector<GLfloat> outputData(NUM_ELEMENTS);

    for (GLuint i = 0; i < NUM_ELEMENTS; ++i)
    {
        inputDataA[i] = static_cast<GLfloat>(i);
        inputDataB[i] = static_cast<GLfloat>(i);
    }

    elementWiseProduct(inputDataA, inputDataB, outputData);

    // Bind and initialize input buffers
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, dataBuffers[0], 0,
                      NUM_ELEMENTS * sizeof(GLfloat));
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, NUM_ELEMENTS * sizeof(GLfloat),
                    inputDataA.data());

    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, dataBuffers[1], 0,
                      NUM_ELEMENTS * sizeof(GLfloat));
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, NUM_ELEMENTS * sizeof(GLfloat),
                    inputDataB.data());

    // Bind output buffer
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, dataBuffers[2], 0,
                      NUM_ELEMENTS * sizeof(GLfloat));

    glUseProgram(computeProgram);
    glDispatchCompute(1, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glFinish();

    GLfloat* ptr;
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, dataBuffers[2], 0,
                      NUM_ELEMENTS * sizeof(GLfloat));
    ptr = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,
                                     NUM_ELEMENTS * sizeof(GLfloat),
                                     GL_MAP_READ_BIT);

    std::cout << "Shader results: " << "\n";
    for (GLuint i = 0; i < NUM_ELEMENTS; i++)
    {
        std::cout << *ptr++ << ", ";
    }

    std::cout << "\nCPU results: " << "\n";
    for (GLuint i = 0; i < NUM_ELEMENTS; i++)
    {
        std::cout << outputData[i] << ", ";
    }

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


    // Delete GL objects
    glDeleteBuffers(3, dataBuffers);
    glDeleteProgram(computeProgram);

    glfwTerminate();

    return 0;
}


void error_callback(GLint error, const GLchar* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << "\n";
}

void elementWiseProduct(const std::vector<GLfloat>& a,
                        const std::vector<GLfloat>& b,
                        std::vector<GLfloat>& output)
{
    for (GLsizei i = 0; i < NUM_ELEMENTS; ++i)
    {
        output[i] = a[i] * b[i];
    }
}
