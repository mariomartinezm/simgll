#include <iostream>
#include <cstdlib>
#include <cmath>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderprogram.h"
#include "camera.h"

constexpr GLuint WIDTH                = 512;
constexpr GLuint HEIGHT               = 512;
constexpr GLuint PARTICLE_GROUP_SIZE  = 512;
constexpr GLuint PARTICLE_GROUP_COUNT = 4096;
constexpr GLint  PARTICLE_COUNT       = PARTICLE_GROUP_SIZE * PARTICLE_GROUP_COUNT;
constexpr GLuint MAX_ATTRACTORS       = 64;

void error_callback(GLint error, const GLchar* description);

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK)
    {
        std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";

        exit(1);
    }

    // Generate two buffers, bind them and initialize their data stores
    GLuint buffers[3], vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(3, buffers);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * PARTICLE_COUNT * sizeof(GLfloat),
                 nullptr, GL_DYNAMIC_COPY);

    GLfloat* positions = static_cast<GLfloat*>(glMapBufferRange(GL_ARRAY_BUFFER,
                                                                0,
                                                                4 * PARTICLE_COUNT * sizeof(GLfloat),
                                                                GL_MAP_WRITE_BIT |
                                                                GL_MAP_INVALIDATE_BUFFER_BIT));

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    for(GLint i = 0; i < PARTICLE_COUNT; i++)
    {
        *positions++ = (dist(engine) - 0.5f) * 1.0f;
        *positions++ = (dist(engine) - 0.5f) * 1.0f;
        *positions++ = (dist(engine) - 0.5f) * 1.0f;
        *positions++ = dist(engine);
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Initialization of the velocity buffer - also filled with random vectors
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * PARTICLE_COUNT * sizeof(GLfloat),
                 nullptr, GL_DYNAMIC_COPY);

    GLfloat* velocities = static_cast<GLfloat*>(glMapBufferRange(GL_ARRAY_BUFFER,
                                                                 0,
                                                                 4 * PARTICLE_COUNT * sizeof(GLfloat),
                                                                 GL_MAP_WRITE_BIT |
                                                                 GL_MAP_INVALIDATE_BUFFER_BIT));

    for(GLuint i = 0; i < PARTICLE_COUNT; i++)
    {
        velocities[4 * i]     = (static_cast<GLfloat>(rand()) / RAND_MAX - 0.5f) / 5.0f;
        velocities[4 * i + 1] = (static_cast<GLfloat>(rand()) / RAND_MAX - 0.5f) / 5.0f;
        velocities[4 * i + 2] = (static_cast<GLfloat>(rand()) / RAND_MAX - 0.5f) / 5.0f;
        velocities[4 * i + 3] = 0.0f;
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    GLuint tbos[2];
    glGenTextures(2, tbos);

    for(GLuint i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_BUFFER, tbos[i]);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffers[i]);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, buffers[2]);
    glBufferData(GL_UNIFORM_BUFFER, 32 * 4 * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);

    // Attractor ubo
    float attractorMasses[MAX_ATTRACTORS];

    for(GLuint i = 0; i < MAX_ATTRACTORS; i++)
    {
        attractorMasses[i] = 0.5f + (static_cast<GLfloat>(rand()) / RAND_MAX) * 0.5f;
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, buffers[2]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    simgll::ShaderProgram computeProgram;
    computeProgram.addShader("compute_shader.glsl", GL_COMPUTE_SHADER);
    computeProgram.compile();

    GLint dtLocation = computeProgram.getLocation("dt");

    simgll::ShaderProgram renderProgram;
    renderProgram.addShader("vertex_shader.glsl",   GL_VERTEX_SHADER);
    renderProgram.addShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
    renderProgram.compile();

    GLint mvpLocation = renderProgram.getLocation("mvp");

    simgll::Camera camera(window,
                          glm::vec3{ 0.0f, 0.0f,  0.0f },
                          glm::vec3{ 0.0f, 0.0f, -1.0f },
                          glm::vec3{ 0.0f, 1.0f,  0.0f });

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPointSize(2.0f);

    GLfloat currentTime = 0.0f;
    GLfloat deltaTime   = 0.0f;
    GLfloat oldTime     = 0.0f;

    while(!glfwWindowShouldClose(window))
    {
        currentTime = static_cast<float>(glfwGetTime());
        deltaTime   = currentTime - oldTime;
        oldTime     = currentTime;

        glfwPollEvents();

        // Update the buffer containing the attractor positions and masses
        GLfloat* attractors = static_cast<GLfloat*>(glMapBufferRange(GL_UNIFORM_BUFFER,
                                                                     0,
                                                                     32 * 4 * sizeof(GLfloat),
                                                                     GL_MAP_WRITE_BIT |
                                                                     GL_MAP_INVALIDATE_BUFFER_BIT));

        for(GLuint i = 0; i < 32; i++)
        {
            attractors[4 * i]     = sinf(deltaTime * static_cast<GLfloat>(i + 4) * 7.5f * 20.0f) * 50.0f;
            attractors[4 * i + 1] = cosf(deltaTime * static_cast<GLfloat>(i + 7) * 3.9f * 20.0f) * 50.0f;
            attractors[4 * i + 2] = sinf(deltaTime * static_cast<GLfloat>(i + 3) * 5.3f * 20.0f) *
                                    cosf(deltaTime * static_cast<GLfloat>(i + 5) * 9.1f) * 100.0f;
            attractors[4 * i + 3] = attractorMasses[i];
        }

        glUnmapBuffer(GL_UNIFORM_BUFFER);

        // Activate the compute program and bind the position and velocity
        // buffers
        computeProgram.use();

        glBindImageTexture(0, tbos[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(1, tbos[1], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        glUniform1f(dtLocation, 1.0f);

        glDispatchCompute(PARTICLE_GROUP_COUNT, 1, 1);

        // Ensure that writes by the compute shader have completed
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        renderProgram.use();

        auto mvp = camera.update(deltaTime, 45.0F, 0.1F, 1000.0F);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

        glBindVertexArray(vao);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}

void error_callback(GLint error, const GLchar* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << "\n";
}
