#include <iostream>
#include <cstdlib>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaderprogram.h"

constexpr GLuint WIDTH = 512, HEIGHT = 512;
constexpr GLuint NUM_ELEMENTS = 2048;

void error_callback(GLint error, const GLchar* description);
void prefix_sum(const float* input, float* output, int elements);

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

    simgll::ShaderProgram computeProgram;
    computeProgram.addShader("compute_shader.glsl", GL_COMPUTE_SHADER);
    computeProgram.compile();

    GLuint dataBuffers[2];

    glGenBuffers(2, dataBuffers);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffers[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_ELEMENTS * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffers[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_ELEMENTS * sizeof(float), nullptr, GL_DYNAMIC_COPY);

    std::vector<GLfloat> inputData(NUM_ELEMENTS);
    std::vector<GLfloat> outputData(NUM_ELEMENTS);

    for(GLuint i = 0; i < NUM_ELEMENTS; i++)
    {
        inputData[i] = static_cast<float>(i);
    }

    prefix_sum(inputData.data(), outputData.data(), NUM_ELEMENTS);

    glShaderStorageBlockBinding(computeProgram.name(), 0, 0);
    glShaderStorageBlockBinding(computeProgram.name(), 1, 1);

    while(!glfwWindowShouldClose(window))
    {
        float* ptr;

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, dataBuffers[0], 0,
                          NUM_ELEMENTS * sizeof(GLfloat));
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, NUM_ELEMENTS *
                        sizeof(GLfloat), inputData.data());

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, dataBuffers[1], 0,
                          NUM_ELEMENTS * sizeof(GLfloat));

        computeProgram.use();
        glDispatchCompute(1, 1, 1);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glFinish();

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, dataBuffers[1], 0,
                          NUM_ELEMENTS * sizeof(GLfloat));
        ptr = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,
                                         NUM_ELEMENTS * sizeof(GLfloat),
                                         GL_MAP_READ_BIT);

        for(GLuint i = 0; i < 15; i++)
        {
            std::cout << *ptr++ << ", ";
        }

        std::cout << "\n";

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    glDeleteBuffers(2, dataBuffers);

    glfwTerminate();

    return 0;
}

void error_callback(GLint error, const GLchar* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << "\n";
}

void prefix_sum(const float* input, float* output, int size)
{
    float f = 0.0f;

    for (GLint i = 0; i < size; i++)
    {
        f += input[i];
        output[i] = f;
    }

    for (GLint i = 0; i < 15; i++)
    {
        std::cout << output[i] << ", ";
    }

    std::cin.get();
}
